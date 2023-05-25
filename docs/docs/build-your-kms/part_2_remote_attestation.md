# Part 2 : Adding the remote attestation 

!!! warning "This part CANNOT be done in simulation mode"


## Remote Attestation Theory

Remote attestation is a security mechanism that enables a **remote entity** to verify the **integrity and authenticity** of a system or application running on another machine. This mechanism can be used to ensure that the system or application is running in a trusted environment and has not been tampered with by a malicious attacker.

**In Intel SGX**, we achieve the remote attestation by generating a enclave **report**. That report is then used to generate the **quote**, which represents its signature. 

The **quote** in Intel SGX represents a digitally signed attestation generated through a hardware and software configuration of a particular SGX enclave. It is the signature that gives proof of the integrity of the application and system (software and hardware evidence). 

It is the quote (partly) that verifies the integrity of the code inside the enclave and that it's really an application enclave running with Intel SGX protections on a trusted Intel SGX platform.

### How does it work?

In remote attestation, [as explained by the Internet Engineering Task Force (IETF) team](https://ietf-rats-wg.github.io/architecture/draft-ietf-rats-architecture.html):

>*One peer (the "Attester") produces believable information about itself - Evidence - to enable a remote peer (the "Relying Party") to decide whether to consider that Attester a trustworthy peer or not. [Remote Attestation procedures] are facilitated by an additional vital party, the Verifier*.

So, remote attestation is a security mechanism used to ensure the integrity of a computing system and its software components. It works by verifying that a system has not been compromised by checking its hardware and software configurations against a trusted set of measurements.

The procedure for remote attestation typically involves three parties: the verifier, the attester, and the challenger. The verifier is the entity that wants to verify the integrity of the attester's system. The attester is the system being verified. The challenger is a trusted third party that provides the verifier with the necessary information to verify the attester's system.

The procedure works as follows: 

1. The ***attester*** generates a ***set of measurements that describe its hardware and software configurations and sends them to the verifier***. 
2. The ***verifier*** then compares ***these measurements against a trusted set of measurements provided by the challenger***.
3. **If the measurements match**, the **verifier** can be confident that the attester's **system has not been compromised**.

Intel SGX's approach to remote attestation is the same but there are too many details to cover to explain it here. If you are interested, though, we wrote an in-depth article about it that you can find [here](https://blindai.mithrilsecurity.io/en/latest/docs/security/remote_attestation/). 

Luckily, Open Enclave has tried to simplify this approach to make their solution more usable, so we don't need to understand all the subtleties to continue!

### Open Enclave's Attestation

The **Open Enclave** community tried to develop a way that's more friendly to the Remote attestation procedures (RATS) specifications. This resulted in an **attestation API**. This API gives a set of functions to generate reports, evidence, and handles all the attestation interface for us. The Open Enclave SDK uses the functions to get evidence and to verify it.

- `openenclave/enclave.h`: contains Open Enclave SDK APIs for creating and managing enclaves which we've already used.
- `openenclave/attestation/attester.h`: provides functions to perform remote attestation and to verify attestation evidence. We will need this for generating the evidence.  
- `openenclave/attestation/sgx/evidence.h`: defines structures and functions for attestation evidence, specifically for Intel SGX Enclaves.
- `openenclave/attestation/sgx/report.h`: provides functions for generating reports that attest to the current state of an SGX Enclave. 

### The implementation

We will be implementing two different concepts to show the difference between two different possible implementations. 

- **Generating the report** inside the enclave : The usual implementation for Intel SGX, is to generate the running enclave's report,  and sign it outside using the Quoting Enclave (Which is one of the architectural enclaves). This quote is then used to be verified by the third-party. 

- **Generating an evidence**. As defined by the IETF:
    >   Evidence is a set of Claims about the Target Environment that reveal operational status, health, configuration, or construction that have security relevance. 

    In Open Enclave, we have the possibility to generate this evidence that is conform to the RFC standardization. A verifier can then take this evidence and compute it's results even if the format is different (it can be a JWT, X.509 certificate or other). 

____________________________________________________________

## AESM service and setup

To start, we'll only need to verify that the AESM service is up and running. It is necessary to contact the architectural enclaves, and achieve a functioning remote attestation. 

To do that, we use `service` command : 

```bash 
$ sudo service aesmd status
● aesmd.service - Intel(R) Architectural Enclave Service Manager
     Loaded: loaded (/lib/systemd/system/aesmd.service; enabled; vendor preset: enabled)
     Active: active (running) since Tue 2023-04-11 11:27:11 UTC; 4 weeks 1 days ago
   Main PID: 764 (aesm_service)
      Tasks: 4 (limit: 9529)
     Memory: 18.6M
     CGroup: /system.slice/aesmd.service
             └─764 /opt/intel/sgx-aesm-service/aesm/aesm_service
```

If the service is active as presented, you good to go, else, you can try restarting the service : 

```bash
$ sudo service aesmd restart
```

____________________________________________________________

## Evidence generation

The evidence generation process begins by retrieving the necessary information from the enclave. For that purpose, Open Enclave SDK has an enclave **implementation** that's dedicated. 

The first function is `oe_get_report`. It creates a report to be used in attestation. It's important to note that the call must be done **inside** the enclave as it is specific to the platform (and each enclave in that sense). 

The second one is `oe_get_evidence`, to generate the evidence.

### Adding `ecall`s

We will be adding two different Ecalls. 

+ `get_report` : will extract the report inside the enclave.
+ `get_evidence_with_pub_key` : will be generating the evidence. 

To do so, we'll need to change our `kms.edl` file. In it, we'll define the Ecalls and add some structures that we will be working with:

```c++
// kms.edl
enclave {
// ...
    struct format_settings_t
    {
        [size=size] uint8_t* buffer;
        size_t size;
    };

    struct pem_key_t
    {
        [size=size] uint8_t* buffer;
        size_t size;
    };

    struct evidence_t
    {
        [size=size] uint8_t* buffer;
        size_t size;
    };

    struct message_t
    {
        [size=size] uint8_t* data;
        size_t size;
    };

    trusted {
        // Untrusted port
        public int set_up_server([in, string] const char* port, bool keep_server_up);
        
        // Extract the evidence from the enclave
        public int get_evidence_with_pub_key(
            [in] const oe_uuid_t* format_id, 
            [in] format_settings_t* format_settings, 
            [out] pem_key_t *pem_key,
            [out] evidence_t *evidence
        );


        // Extract the enclave's report and public key for the remote attestation
        public int get_report(
            [out] uint8_t **pem_key,
            [out] size_t *key_size, 
            [out] uint8_t **report, 
            [out] size_t *report_size
        );
    };

//...

}
```

***# WHAT DO YOU MEAN WITH THE FOLLOWING PARAGRAPH? BIT CONFUSED BY WHAT YOU MEAN BY JUST COPIED AS IS. ISN'T EVERYTHING COPIED AS IS? [yass] It's the variable that is copied into a shared memeory I'm going to add some explanation ***

If we remember correctly, when we first presented the [trusted and untrusted sections](./trusted-vs-untrusted-impl.md), we talked about parameter boundary. But up until know we only talked about the `in` boundary. But the `in` boundary only copies the value of the pointer that has been given. To pass in a value that must be modified by the enclave we use the `out` boundary. 
This Figure from the intel WhitePaper explains how it works more precisely :
![Inbound and Outbound boundaries](../assets/outbound_inbound.png)

In the `get_evidence_with_pub_key` function, `format_id` and `format_settings` are just copied as is. Those two variables represents the settings that will passed on to the enclave to generate the right evidence (such as the ECDSA-key generation format). 

***# THOUGHT. I KNOW THIS IS VERY BASIC CRYPTO BUT SHOULD WE EXPLAIN ABOUT ECDSA-key OR ADD A WORD SOMEWHERE TO MAKE IT MORE CLEAR? OR MAYBE WHAT THE KEY WILL BE FOR?[yass] I don't know haha, we can't explain everything at some point***

This function adds the public key in PEM format and the report in the enclave and copies it in the four variables (hence the outbound `out`). 

### Remote Attestation and evidence generation structure

To get the integrity and confidentiality information inside the enclave, we'll build the `Attestation` structure. It will allow us to add sequentially all the functions that we will be needing to gather them. 

First and foremost, we will start by creating a folder called `common`, that will have three different **class object definition** : crypto, attestation and dispatcher.

```bash
$ mkdir common && cd common && touch crypto.cpp crypto.h attestation.cpp attestation.h dispatcher.cpp dispatcher.h
```

- `crypto`: This class object will contain all the cryptographic operations that will needed in the attestation operation (such as hashing, encrypting & decrypting). It's pretty much a new (better) version of the functions that we've implemented in Part I. 

- `attestation` : This class object will contain the methods that will retrieve the cryptographic proof needed to be sent to the host. It will be using the crypto object to use some cryptographic operations directly. 

- `dispatcher` : This class object will handle to communication between the Ecalls and the attestation class.

So, let's start by adding the `get_report` Ecall to retrieve the enclave report. 

### The `crypto` class object 

The `crypto` class object provides functionality for encryption and hashing using the **RSA algorithm** and **SHA-256 hash** function. It uses the **MbedTLS** library for cryptographic operations. 

It is not the purpose of our tutorial to go over the details of this class object, so you can copy the files from the mini-KMS repo to yours.

You can copy them from [here](https://github.com/mithril-security/Confidential_Computing_Explained/tree/main/mini_kms/part_2/common). 

??? note "More information about the crypto class object"

    1. `Crypto::Crypto()`: Constructor method that initializes the crypto module by calling `init_mbedtls()`.

    2. `Crypto::~Crypto()`: Destructor method that frees resources allocated by the crypto module by calling `cleanup_mbedtls()`.

    3. `Crypto::init_mbedtls()`: Method that initializes the crypto module by performing the following operations:

		- Initializes the `m_entropy_context`, `m_ctr_drbg_context`, and `m_pk_context` structures from the mbedtls library.
		- Seeds the `m_ctr_drbg_context` structure with entropy using `mbedtls_ctr_drbg_seed()` function.
		- Sets up an RSA key pair of **2048-bit** with exponent **65537** using `mbedtls_rsa_gen_key()` function.
		- Writes out the public key in PEM format using `mbedtls_pk_write_pubkey_pem()` function.

    4. `Crypto::cleanup_mbedtls()`: Method that frees resources allocated by the crypto module by calling the corresponding *mbedtls* cleanup functions (`mbedtls_pk_free()`, `mbedtls_entropy_free()`, and `mbedtls_ctr_drbg_free()`).

    5. `Crypto::retrieve_public_key()`: Method that retrieves the public key of the enclave by copying the value of `m_public_key` to the `pem_public_key` buffer provided.

    6. `Crypto::Sha256()`: Method that computes the *SHA256* hash of the provided data using the mbedtls library functions (`mbedtls_sha256_init()`, `mbedtls_sha256_starts_ret()`, `mbedtls_sha256_update_ret()`, and `mbedtls_sha256_finish_ret()`).

    7. `Crypto::Encrypt()`: Method that encrypts the provided data using the public key of another enclave. The method performs the following operations:

		- Parses the provided public key into an `mbedtls_pk_context` structure using `mbedtls_pk_parse_public_key()` function.
		- Sets the RSA padding and hash algorithm to be used for encryption.
		- Encrypts the data using `mbedtls_rsa_pkcs1_encrypt()` function with the parsed public key.
		- Sets the encrypted data size and returns `true` if successful.


### The `attestation` class object 

As a reminder: the `attestation` class object will contain the methods that will retrieve the cryptographic proof needed to be sent to the host. 

### Structure and definition

Let's start by writing the class's structure and definition:

```c++
#include "attestation.h"
#include <openenclave/attestation/attester.h>
#include <openenclave/attestation/custom_claims.h>
#include <openenclave/attestation/verifier.h>
#include <openenclave/bits/report.h>
#include <string.h>
#include "../enclave/trace.h"

Attestation::Attestation(Crypto* crypto)
{
    m_crypto = crypto;
}
```

Now, let's add an Attestation object. This **Attestation** object will implement functions for attestation: `generate_attestation_evidence` and `generate_report`.

#### `generate_attestation_evidence`

The `generate_attestation_evidence` method will generate evidence for attestation, which is a cryptographic proof of the *integrity* and *authenticity* of an enclave. The function takes in several parameters including `format_id`, `format_settings`, `data`, and `data_size`. 


```C++
bool Attestation::generate_attestation_evidence(
    const oe_uuid_t* format_id, 
    uint8_t* format_settings, 
    size_t format_settings_size, 
    const uint8_t* data, 
    const size_t data_size, 
    uint8_t **evidence, 
    size_t *evidence_size)
{
    bool ret = false; 
    uint8_t hash[32];
    oe_result_t result = OE_OK; 
    // custom claims 
    uint8_t* custom_claims_buffer = nullptr; 
    size_t custom_claims_buffer_size = 0; 
    char custom_claim1_name[] = "Event";
    char custom_claim1_value[] = "Attestation KMS example";
    char custom_claim2_name[] = "Public key hash";

    oe_claim_t custom_claims[2] = {
        {
            .name = custom_claim1_name,
            .value = (uint8_t*)custom_claim1_value,
            .value_size = sizeof(custom_claim1_value)
        },
        {
            .name = custom_claim2_name,
            .value = nullptr,
            .value_size = 0
        }
    };

    // 1. first hashing the input data using SHA256. 
    if (m_crypto->Sha256(data, data_size, hash) != 0)
    {
        TRACE_ENCLAVE("data hashing failed !\n");
        goto exit; 
    }

    // 2. Then, initialize the attester and plugin by calling `oe_attester_initialize()`.
    result = oe_attester_initialize();
    if (result != OE_OK)
    {
        TRACE_ENCLAVE("oe_attester_initialize failed !\n");
        goto exit; 
    }

    // 3. Next, generates custom claims for the attestation.
    custom_claims[1].value = hash;
    custom_claims[1].value_size = sizeof(hash);

    // 4. Serialize the custom claims using `oe_serialize_custom_claims`.
    TRACE_ENCLAVE("Serializing the custom claims.\n");
    if (oe_serialize_custom_claims(
        custom_claims, 
        2, 
        &custom_claims_buffer, 
        &custom_claims_buffer_size
    ) != OE_OK)
    {
        TRACE_ENCLAVE("oe_serialize_custom_claims failed !\n");
        goto exit;
    }

    TRACE_ENCLAVE(
    "serialized custom claims buffer size: %lu", custom_claims_buffer_size);

    // 5. Call to oe_get_evidence function to generate the evidence with the format chosen by the attester
    result = oe_get_evidence(
        format_id,
        0,
        custom_claims_buffer,
        custom_claims_buffer_size,
        format_settings,
        format_settings_size,
        evidence,
        evidence_size,
        nullptr,
        0);
    if (result != OE_OK)
    {
        TRACE_ENCLAVE("oe_get_evidence failed.(%s)", oe_result_str(result));
        goto exit;
    }

    ret = true;
    TRACE_ENCLAVE("generate_attestation_evidence succeeded.");
exit:
    // 6. Finally, clean up and returns a boolean indicating whether the function succeeded or failed.
    oe_attester_shutdown();
    return ret;
}  

```

#### `generate_report`

The `generate_report` method generates a remote report for the given data. The SHA256 digest of the data is stored in the `report_data` field of the generated remote report. 





```c++
bool Attestation::generate_report(
    const uint8_t* data,
    const size_t data_size,
    uint8_t** remote_report_buf,
    size_t* remote_report_buf_size)
{
    bool ret = false;
    uint8_t sha256[32];
    oe_result_t result = OE_OK;
    uint8_t* temp_buf = NULL;

    // 1. Firstly, it hashes the input data using SHA256, and then generates a remote report using `oe_get_report`. 
    if (m_crypto->Sha256(data, data_size, sha256) != 0)
    {
        goto exit;
    }


    // 2. It sets the `OE_REPORT_FLAGS_REMOTE_ATTESTATION` flag to generate a remote report that can be attested remotely by an enclave running on a different platform. 
    result = oe_get_report(
        OE_REPORT_FLAGS_REMOTE_ATTESTATION,
        sha256, // Store sha256 in report_data field
        sizeof(sha256),
        NULL, // opt_params must be null
        0,
        &temp_buf,
        remote_report_buf_size);
    if (result != OE_OK)
    {
        TRACE_ENCLAVE("oe_get_report failed.");
        goto exit;
    }
    *remote_report_buf = temp_buf;
    ret = true;
    TRACE_ENCLAVE("generate_remote_report succeeded.");
exit:

    // 3. Finally, it cleans up and returns a boolean indicating whether the function succeeded or failed.
    return ret;
}
```

### The dispatcher

The `dispatcher` dispatches the `attestation` and `crypto` object to be called when our Ecalls will be defined. For each one of our Ecalls, we defined a method that sets up everything for the evidence generation and uses the `get_evidence` function method and a another function for `get_report` method:

```C++
/**
* Dispatcher class to be called from the enclave to run the ecalls. 
*/
class dispatcher
{
  private:
    bool m_initialized;
    Crypto* m_crypto;
    Attestation* m_attestation;
    string m_name;

  public:
    // Constructor
    dispatcher(const char* name);
    // Destructor
    ~dispatcher();

    // call to last section's report function in attestation
    int get_remote_report_with_pubkey(
        uint8_t** pem_key,
        size_t* key_size,
        uint8_t** remote_report,
        size_t* remote_report_size);

    // call to last section's evidence function in attestation
    int get_evidence_with_pubkey(
      const oe_uuid_t* format_id, 
      format_settings_t* format_settings, 
      pem_key_t* pem_key, 
      evidence_t* evidence
    );

  private:
    // Initializes a name for the enclave
    bool initialize(const char* name);
};
```

!!! note "You can find an example of the implementation in our [repo](https://github.com/mithril-security/Confidential_Computing_Explained/tree/mini_kms/mini_kms/part_2)." 

____________________________________________________________

## Changes to the enclave code

Before writing the Ecall function, we have to make some changes to the Makefile to link and compile our new files.


```Makefile
# enclave/Makefile

include ../config.mk

CRYPTO_LDFLAGS := $(shell pkg-config oeenclave-$(COMPILER) --variable=${OE_CRYPTO_LIB}libs)

ifeq ($(LVI_MITIGATION), ControlFlow)
    ifeq ($(LVI_MITIGATION_BINDIR),)
        $(error LVI_MITIGATION_BINDIR is not set)
    endif
    # Only run once.
    ifeq (,$(findstring $(LVI_MITIGATION_BINDIR),$(CC)))
        CC := $(LVI_MITIGATION_BINDIR)/$(CC)
    endif
    COMPILER := $(COMPILER)-lvi-cfg
    CRYPTO_LDFLAGS := $(shell pkg-config oeenclave-$(COMPILER) --variable=${OE_CRYPTO_LIB}libslvicfg)
endif

CFLAGS=$(shell pkg-config oeenclave-$(COMPILER) --cflags)
CXXFLAGS=$(shell pkg-config oeenclave-$(CXX_COMPILER) --cflags) # Added remote attestation
LDFLAGS=$(shell pkg-config oeenclave-$(CXX_COMPILER) --libs)
INCDIR=$(shell pkg-config oeenclave-$(COMPILER) --variable=includedir)

all:
	$(MAKE) build
	$(MAKE) keys
	$(MAKE) sign
# adding the 	$(CXX) -g -c $(CXXFLAGS) -I. -I.. -std=c++11 -DOE_API_VERSION=2 enclave.cpp ../common/attestation.cpp ../common/crypto.cpp ../common/dispatcher.cpp line to add our classes 
# must be compiled in C++
build:
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../kms.edl --trusted \
		--search-path $(INCDIR) \
		--search-path $(INCDIR)/openenclave/edl/sgx
	$(CXX) -g -c $(CXXFLAGS) -I. -I.. -std=c++11 -DOE_API_VERSION=2 enclave.cpp ../common/attestation.cpp ../common/crypto.cpp ../common/dispatcher.cpp 
	$(CC) -g -c $(CFLAGS) -DOE_API_VERSION=2 kms_t.c -o kms_t.o
	$(CC) -g -c $(CFLAGS) -DOE_API_VERSION=2 ../mongoose/mongoose.c -lmbedtls -lmbedcrypto -lmbedx509 -D MG_ENABLE_MBEDTLS=1 -o mongoose.o
	$(CXX) -o enclave kms_t.o mongoose.o enclave.o -D MG_ENABLE_MBEDTLS=1 -loelibcxx -loehostsock -loehostresolver -loehostepoll $(LDFLAGS) $(CRYPTO_LDFLAGS) 

sign:
	oesign sign -e enclave -c kms.conf -k private.pem

clean:
	rm -f mongoose.o attestation.o crypto.o dispatcher.o enclave.o enclave enclave.signed private.pem public.pem kms_t.o kms_t.h kms_t.c kms_args.h

keys:
	openssl genrsa -out private.pem -3 3072
	openssl rsa -in private.pem -pubout -out public.pem


```

Next we can return to the `enclave.cpp` file and start writing our ecalls : 

```C++
int get_report(uint8_t **pem_key, size_t *key_size, uint8_t **report, size_t *report_size){
    TRACE_ENCLAVE("Entering enclave.\n");
    TRACE_ENCLAVE("Modules loading...\n");
    if (load_oe_modules() != OE_OK)
    {
        printf("loading required Open Enclave modules failed\n");
        return -1;
    }
    TRACE_ENCLAVE("Modules loaded successfully.\n");
    TRACE_ENCLAVE("Calling sgx attester init.\n");
    oe_result_t result = OE_OK;
    result = oe_attester_initialize();
    TRACE_ENCLAVE("Calling sgx plugin attester.\n");

    TRACE_ENCLAVE("Calling get report through dispatcher.\n");
    
    return dispatcher.get_remote_report_with_pubkey(pem_key, key_size, report, report_size);
}

int get_evidence_with_pub_key(
    const oe_uuid_t* format_id,
    format_settings_t* format_settings,
    pem_key_t* pem_key,
    evidence_t* evidence)
{
    TRACE_ENCLAVE("Entering enclave.\n");
    TRACE_ENCLAVE("Modules loading...\n");
    if (load_oe_modules() != OE_OK)
    {
        printf("loading required Open Enclave modules failed\n");
        return -1;
    }
    TRACE_ENCLAVE("Modules loaded successfully.\n");
    TRACE_ENCLAVE("Running get evidence with public key through dispatcher.\n");

    return dispatcher.get_evidence_with_public_key(
        format_id, format_settings, pem_key, evidence);
}
```

And that's pretty much it for the Ecalls! Let's move on to calling these functions from the host.

____________________________________________________________

## Changes to the Host code

### The report and quote generation
Let's first add the `get_report ecall` to the dispatcher from the host. 
We first define the variables that we will be using, so let's add the following variables on the `main` function: 
```c++
// host.cpp
    uint8_t *pem_key = NULL;
    size_t key_size = NULL;
    uint8_t *report = NULL;
    size_t report_size = NULL; 
    oe_report_t parsed_report = {0};
```

next we can call to `get_report` after creating the enclave: 
```c++
    result = get_report(enclave, &ret, &pem_key, &key_size, &report, &report_size);
    if ((result != OE_OK) || (ret != 0))
    {
        printf("[Host]: get_report failed.");
        if (ret==0)
        ret =1;
        goto exit;
    }  
```

At this stage, our report is not yet parse. To make it into a real report structure as defined by Intel, we are going to use `oe_parse_report` to do so. 
But what we actually need is a Quote. As defined by Intel SGX, the quote, which will be used to verify the platform, is a signed report. Technically, this quote is generated by the Quoting Enclave (which is one of the five architectural enclaves), which possesses the keys to sign the report. This quote is then used by the verifier to be checked as a sure format and platform. 

We instantiate the `QuoteGeneration` object that parses the results in JSON (the code can be found at `mini_kms/part_2/host/quoteGeneration.cpp`). 

We can complete the quote generation by add the following code to complete the quote generation: 
```c++
   printf("[Host]: Parsing enclave report.\n");
    result = oe_parse_report(report, report_size, &parsed_report);

    if ((result != OE_OK))
    {
        printf("[Host]: Parsing report failed.");
        goto exit;
    }  
    else{
            printf("[Host]: Begining quote Generation.\n");
            QuoteGeneration quotegen(parsed_report, report, report_size, pem_key, key_size);
            quotegen.PrintToJson(stdout); // This will print the data in the quotegen object
    }
```

The results we get are the following: 
```
{
  "Type": 2,
  "MrEnclaveHex": "AF3438654B544D7E94B6B538EAF58528A2136F6A107B95B4C442B0AFC9C98D83",
  "MrSignerHex": "58272DD3DB731D223107A2CD8A9522D84D25AAED904BF6B2345A8AF9F9BFBF6D",
  "ProductIdHex": "01000000000000000000000000000000",
  "SecurityVersion": 1,
  "Attributes": 3,
  "QuoteHex": "01000000020000007A12000000000000030002000000000009000D00939A7233F79C4CA9940A0DB3957F060744373EAAF0FB08878E4A4B1EA8A924B8000000000B0B100FFFFF01000000000000000000000000000000000000000000000000000000000000000000000000000000000007000000000000000700000000000000AF3438654B544D7E94B6B538EAF58528A2136F6A107B95B4C442B0AFC9C98D83000000000000000000000000000000000000000000000000000000000000000058272DD3DB731D223107A2CD8A9522D84D25AAED904BF6B2345A8AF9F9BFBF6D000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002DF55AC254488A87DEA543BA412C746848FDE4EBA1A16260377EB1096929809C0000000000000000000000000000000000000000000000000000000000000000C6100000A769761B043C00DCB0B47D3720612C6CCB7B33368828E5F844388A717C2AEED21952D011FC12C6D024C4625A2CBA3C94E46828D51BFC6F8D68FEBD8B2A5A3AF91789AF146C7D37C84E00A4CD4FFCEF4B40630D764DFC533DAE8E66ECF3BD19A2C27D130A086D9C0B61B3007E9853D1E03A027CF359EB520C71541973AC02E3E80B0B100FFFFF0100000000000000000000000000000000000000000000000000000000000000000000000000000000001500000000000000E700000000000000CE1DA89AC1F54A807257C4E57C78140CBC6652D4D587D60F0583125A2792BE7000000000000000000000000000000000000000000000000000000000000000008C4F5775D796503E96137F77C68A829A0056AC8DED70140B081B094490C57BFF0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100090000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000064255625ADBC80954B91CD1E327A8970658EDB3C73CAC3BD6A5A98D5988DF4270000000000000000000000000000000000000000000000000000000000000000FFA12416A46F35452C505F30EBF1E4DB74D49D1572549E0EFA8508939C10FF2104CCAA5D87F556C1D50FB48F8494DDD8A03AA8E21CB0FB773F7C7C5F9E54E4122000000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F05005E0E00002D2D2D2D2D424547494E2043455254494649434154452D2D2D2D2D0A4D49494538544343424A6967417749424167495542737777382F70384C32644E67757266674536654F69675956424D77436759494B6F5A497A6A3045417749770A634445694D434147413155454177775A535735305A577767553064594946424453794251624746305A6D397962534244515445614D42674741315545436777520A535735305A577767513239796347397959585270623234784644415342674E564241634D43314E68626E526849454E7359584A684D51737743515944565151490A44414A445154454C4D416B474131554542684D4356564D774868634E4D6A4D774E5445774D6A41794E6A41355768634E4D7A41774E5445774D6A41794E6A41350A576A42774D534977494159445651514444426C4A626E526C624342545231676755454E4C49454E6C636E52705A6D6C6A5958526C4D526F77474159445651514B0A4442464A626E526C6243424462334A7762334A6864476C76626A45554D424947413155454277774C553246756447456751327868636D4578437A414A42674E560A4241674D416B4E424D517377435159445651514745774A56557A425A4D424D4742797147534D34394167454743437147534D34394177454841304941424852380A2B7A2B587056314E4D72447330637959312B35306A4B752F6A496B67366A6549375337534C7550744F666C73774D785A574A32516153467A795349752F3774720A5A61385358643042586F6C5749744C706849436A67674D4F4D494944436A416642674E5648534D4547444157674253566231334E765276683655424A796454300A4D383442567776655644427242674E56485238455A4442694D47436758714263686C706F64485277637A6F764C32467761533530636E567A6447566B633256790A646D6C6A5A584D75615735305A577775593239744C334E6E6543396A5A584A3061575A7059324630615739754C33597A4C33426A61324E796244396A595431770A624746305A6D397962535A6C626D4E765A476C755A7A316B5A584977485159445652304F42425945464643665461765A6C536739684651454E6246376E6854420A5A4B6B374D41344741315564447745422F775145417749477744414D42674E5648524D4241663845416A41414D4949434F77594A4B6F5A496876684E415130420A424949434C444343416967774867594B4B6F5A496876684E4151304241515151674A475A7A6467355A43583044424C72714B756D706A434341575547436971470A534962345451454E41514977676746564D42414743797147534962345451454E415149424167454C4D42414743797147534962345451454E415149434167454C0A4D42414743797147534962345451454E41514944416745444D42414743797147534962345451454E41514945416745444D42454743797147534962345451454E0A41514946416749412F7A415242677371686B69472B4530424451454342674943415038774541594C4B6F5A496876684E4151304241676343415145774541594C0A4B6F5A496876684E4151304241676743415141774541594C4B6F5A496876684E4151304241676B43415141774541594C4B6F5A496876684E4151304241676F430A415141774541594C4B6F5A496876684E4151304241677343415141774541594C4B6F5A496876684E4151304241677743415141774541594C4B6F5A496876684E0A4151304241673043415141774541594C4B6F5A496876684E4151304241673443415141774541594C4B6F5A496876684E4151304241673843415141774541594C0A4B6F5A496876684E4151304241684143415141774541594C4B6F5A496876684E4151304241684543415130774877594C4B6F5A496876684E41513042416849450A4541734C4177502F2F7745414141414141414141414141774541594B4B6F5A496876684E4151304241775143414141774641594B4B6F5A496876684E415130420A4241514741474271414141414D41384743697147534962345451454E4151554B415145774867594B4B6F5A496876684E415130424267515165324D416B2B2B450A7172655676502B7851354E6C7044424542676F71686B69472B453042445145484D4459774541594C4B6F5A496876684E4151304242774542416638774541594C0A4B6F5A496876684E4151304242774942415141774541594C4B6F5A496876684E4151304242774D4241514177436759494B6F5A497A6A304541774944527741770A52414967494C716368586D5A7068505569343544306978555577746C786665785461474E6F4E366F4241664E415763434943364156456157435249394E564E690A75576E6752396B7555734C6977446631796C61416D414447435844730A2D2D2D2D2D454E442043455254494649434154452D2D2D2D2D0A2D2D2D2D2D424547494E2043455254494649434154452D2D2D2D2D0A4D4949436C6A4343416A32674177494241674956414A567658633239472B487051456E4A3150517A7A674658433935554D416F4743437147534D343942414D430A4D476778476A415942674E5642414D4D45556C756447567349464E48574342536232393049454E424D526F77474159445651514B4442464A626E526C624342440A62334A7762334A6864476C76626A45554D424947413155454277774C553246756447456751327868636D4578437A414A42674E564241674D416B4E424D5173770A435159445651514745774A56557A4165467730784F4441314D6A45784D4455774D5442614677307A4D7A41314D6A45784D4455774D5442614D484178496A41670A42674E5642414D4D47556C756447567349464E4857434251513073675547786864475A76636D306751304578476A415942674E5642416F4D45556C75644756730A49454E76636E4276636D4630615739754D5251774567594456515148444174545957353059534244624746795954454C4D416B474131554543417743513045780A437A414A42674E5642415954416C56544D466B77457759484B6F5A497A6A3043415159494B6F5A497A6A304441516344516741454E53422F377432316C58534F0A3243757A7078773734654A423732457944476757357258437478327456544C7136684B6B367A2B5569525A436E71523770734F766771466553786C6D546C4A6C0A65546D693257597A33714F42757A43427544416642674E5648534D4547444157674251695A517A575770303069664F44744A5653763141624F536347724442530A42674E5648523845537A424A4D45656752614244686B466F64485277637A6F764C324E6C636E52705A6D6C6A5958526C63793530636E567A6447566B633256790A646D6C6A5A584D75615735305A577775593239744C306C756447567355306459556D397664454E424C6D526C636A416442674E5648513445466751556C5739640A7A62306234656C4153636E553944504F4156634C336C517744675944565230504151482F42415144416745474D42494741315564457745422F7751494D4159420A4166384341514177436759494B6F5A497A6A30454177494452774177524149675873566B6930772B6936565947573355462F32327561586530594A446A3155650A6E412B546A44316169356343494359623153416D4435786B66545670766F34556F79695359787244574C6D5552344349394E4B7966504E2B0A2D2D2D2D2D454E442043455254494649434154452D2D2D2D2D0A2D2D2D2D2D424547494E2043455254494649434154452D2D2D2D2D0A4D4949436A7A4343416A53674177494241674955496D554D316C71644E496E7A6737535655723951477A6B6E42717777436759494B6F5A497A6A3045417749770A614445614D4267474131554541777752535735305A5777675530645949464A766233516751304578476A415942674E5642416F4D45556C756447567349454E760A636E4276636D4630615739754D5251774567594456515148444174545957353059534244624746795954454C4D416B47413155454341774351304578437A414A0A42674E5642415954416C56544D423458445445344D4455794D5445774E4455784D466F58445451354D54497A4D54497A4E546B314F566F77614445614D4267470A4131554541777752535735305A5777675530645949464A766233516751304578476A415942674E5642416F4D45556C756447567349454E76636E4276636D46300A615739754D5251774567594456515148444174545957353059534244624746795954454C4D416B47413155454341774351304578437A414A42674E56424159540A416C56544D466B77457759484B6F5A497A6A3043415159494B6F5A497A6A3044415163445167414543366E45774D4449595A4F6A2F69505773437A61454B69370A314F694F534C52466857476A626E42564A66566E6B59347533496A6B4459594C304D784F346D717379596A6C42616C54565978465032734A424B357A6C4B4F420A757A43427544416642674E5648534D4547444157674251695A517A575770303069664F44744A5653763141624F5363477244425342674E5648523845537A424A0A4D45656752614244686B466F64485277637A6F764C324E6C636E52705A6D6C6A5958526C63793530636E567A6447566B63325679646D6C6A5A584D75615735300A5A577775593239744C306C756447567355306459556D397664454E424C6D526C636A416442674E564851344546675155496D554D316C71644E496E7A673753560A55723951477A6B6E4271777744675944565230504151482F42415144416745474D42494741315564457745422F7751494D4159424166384341514577436759490A4B6F5A497A6A3045417749445351417752674968414F572F35516B522B533943695344634E6F6F774C7550524C735747662F59693747535839344267775477670A41694541344A306C72486F4D732B586F356F2F7358364F39515778485241765A55474F6452513763767152586171493D0A2D2D2D2D2D454E442043455254494649434154452D2D2D2D2D0A00",
  "EnclaveHeldDataHex": "2D2D2D2D2D424547494E205055424C4943204B45592D2D2D2D2D0A4D494942496A414E42676B71686B6947397730424151454641414F43415138414D49494243674B43415145416A552B4C356147716B4F533932792F6B513062570A566631426637657044336F484530486A6A684A74325A627A49585A303245527243555946704857774749546F6B564C6A684C63525665364766775A776A6239680A7566474F584D6D71444F6557356752734856325137354C6E614338654B32364F62775755616A74456352594666353361563961697A745033574449737A796C750A49757059516875564F2B574936506F454E516446686A41424C7A6C787A79374369547A364452796137476155692F596E44564668414664435255324E613447410A79322B596B454A506550646F5441494E7054534C7954476E73346B7555717A375149726E6E3473426D44724C786635744534736447644B68596435525A5A68310A744B32634F3048534174656456574C48466F476571464A35697A62377436424A5546616C4E7A47466173516A36654F796F3050574654334F53766A3357596C4C0A32514944415141420A2D2D2D2D2D454E44205055424C4943204B45592D2D2D2D2D0A00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
}
```

### Generating the evidence 

To get the evidence via the ecall, we declare the `get_enclave_evidence` function: 
```c++
// Attestation's evidence 
int get_enclave_evidence(
    oe_uuid_t* format_id, 
    const char* enclave_name, 
    oe_enclave_t* enclave
)
{
    oe_result_t result = OE_OK; 
    int ret = 1; 
    format_settings_t format_settings = {0}; 
    evidence_t evidence = {0};
    pem_key_t pem_key = {0}; 


    printf("[Host]: Retrieving evidence.\n");
    result = get_evidence(
        enclave,
        &ret,
        format_id,
        &format_settings,
        &pem_key,
        &evidence);
    if ((result != OE_OK) || (ret != 0))
    {
        printf(
            "[Host]: get_evidence failed. %s\n",
            oe_result_str(result));
        if (ret == 0)
            ret = 1;
        goto exit;
    }


    exit:
    free(pem_key.buffer);
    free(evidence.buffer);
    free(format_settings.buffer);
    return ret;
}
```

Let's detail the parameters ppassed on to generate the evidence: 

- `format_id`: defines the UUID of the attestation that we will be demanding. Open Enclave defines 4 different formats for `oe_get_evidence` ( `OE_FORMAT_UUID_SGX_LOCAL_ATTESTATION`, `OE_FORMAT_UUID_SGX_ECDSA` , `OE_FORMAT_UUID_SGX_EPID_LINKABLE`, `OE_FORMAT_UUID_SGX_EPID_UNLINKABLE`). And the one that interests us is the ECDSA one. So, in the main function we instantiate as `OE_FORMAT_UUID_SGX_ECDSA` see main function [in `host.cpp`](https://github.com/mithril-security/Confidential_Computing_Explained/blob/main/mini_kms/part_2/host/host.cpp). 
- `format_settings`: is mostly used for additional information to the verifier and local attestation. But we won't be needing it so we initialize it at 0. 
- `pem_key` and `evidence`: respectively the public key and evidence variables that will be changed. 

Running this function on the `main` function of `host.cpp` will get the evidence. 


__________________ __________________________________________
## What's next? 

Now that we have the data needed to verify the enclave, we must send it to the third-party. To do so, we will be using our little web server that we've put in place in the last chapter. However we will still need to verify the attestation sent and establish a new connection, but this one secured with a certificate generated by the enclave. 

In the next chapter, we will be seeing the following topics: 
- sending the evidence to a client app.
- verify the evidence client side.
- establish a new TLS connection if the attestation is verified called Attested TLS.

<br />
<br />
[Next Chapter Under Construction :fontawesome-solid-hammer:](#){ .md-button .md-button--primary }
