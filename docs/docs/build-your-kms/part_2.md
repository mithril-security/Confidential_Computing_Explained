# Part 2 : Adding the remote attestation to our KMS code

!!! warning
    This part CANNOT be done in simulation mode



## Some theoretical explanation and set-up 
<!-- Remote attestation is a security mechanism that enables a **remote entity** to verify the **integrity and authenticity** of a system or application running on another machine. This mechanism can be used to ensure that the system or application is running in a trusted environment and has not been tampered with by a malicious attacker.

***In Intel SGX***, we achieve the remote attestation by generating a enclave **report** that is then used to generate the **quote** which represents the signature of the report. 

The **quote** in Intel SGX represents a digitally signed attestation generated through a hardware and software configuration of a particular SGX enclave. It is the signature that gives proof of the integrity of the application and system (software and hardware evidence). 

It is the quote (partly) that verifies the integrity of the code inside the enclave and that it's really an application enclave running with Intel SGX protections on a trusted Intel SGX platform.  -->

### How does it work? (briefly)
In Remote attestation, as explained by the IETF team (you can find the whole document [here](https://ietf-rats-wg.github.io/architecture/draft-ietf-rats-architecture.html)),   
>*"one peer (the "Attester") produces believable information about itself - Evidence - to enable a remote peer (the "Relying Party") to decide whether to consider that Attester a trustworthy peer or not. RATS are facilitated by an additional vital party, the Verifier"*.

So, remote attestation is a security mechanism used to ensure the integrity of a computing system and its software components. It works by verifying that a system has not been compromised by checking its hardware and software configurations against a trusted set of measurements.

The procedure for remote attestation typically involves three parties: the verifier, the attester, and the challenger. The verifier is the entity that wants to verify the integrity of the attester's system, while the attester is the system being verified. The challenger is a trusted third party that provides the verifier with the necessary information to verify the attester's system.

The procedure works as follows: The ***attester*** generates a ***set of measurements that describe its hardware and software configurations and sends them to the verifier***. The ***verifier*** then compares ***these measurements against a trusted set of measurements provided by the challenger***. If the measurements match, the verifier can be confident that the attester's system has not been compromised.


Intel SGX's approach to Remote Attestation is the same but demands a lot of detail to be explained in this article. Fortunaletly, if you're looking for that depth of explanation, we've wrote an article about it that you can find [here](https://blindai.mithrilsecurity.io/en/latest/docs/security/remote_attestation/). 


Good thing for us, we won't have to get in that much detail because OpenEnclave have tried to simplifies this approach to make more usable as we'll see next ! 

### Attestation on OpenEnclave

The **Open Enclave** community tried to develop a way that's more friendly to the Remote attestation procedures (RATS) specifications. This resulted in an attestation API. This API gives a set of functions to generate reports, evidence, and handles all the attestation interface for us to be used. The OpenEnclave SDK uses the functions to get evidence and to verify it. 

- `openenclave/enclave.h`: contains Open Enclave SDK APIs for creating and managing Enclaves which we've already used.
- `openenclave/attestation/attester.h`: provides functions to perform remote attestation and to verify attestation evidence, that what we will need for generating the evidence.  
- `openenclave/attestation/sgx/evidence.h`: defines structures and functions for attestation evidence, specifically for Intel Software Guard Extensions (SGX) Enclaves.
- `openenclave/attestation/sgx/report.h`: provides functions for generating reports that attest to the current state of an SGX Enclave. 



### Verifying that all the services are set-up 
verifying that aesm service is up and running. 
The AESM service is necessary to contact the architectural enclaves. And as explained in the previous paragraph, they also be needed to achieve a functionning *remote attestation*. 

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


## The evidence generation
The quote generation process begins by retrieving the report from the enclave. For that purpose, OpenEnclave SDK has an enclave **implementation** that's dedicated. Indeed, the function `oe_get_report` creates a report to be used in atte station (and specifically for our case, remote attestation). This call must be done **inside** the enclave as it is specific to the platform (and each enclave in that sense).

However, to generate the quote, we must do so from the host. Because in the quote, we also have to add to add information specific to the platform, and hence to generate the generate, the host contacts the quoting enclave with the freshly sent report from the enclave. 

When the quote is generate, it must then be sent to the user for verification. 
This is achieved through the web server done previously on part 1. 

### Adding `ecall`s for the remote attestation
We will be adding two different examples, the first one, will be regarding extracting the report inside the enclave and will be represented by the `get_report` ecall. 
In the second one, we will be presenting the evidence that will 
So let's change the `kms.edl` by defining the ecalls and adding some structures that we will be working with.
We are going to add the following:
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

In the `get_evidence_with_pub_key` function, `format_id` and `format_settings` are just copied as it is. Those two variables represents the settings that will passed on to the enclave to generate the right evidence (such as the ECDSA-key generation format). 
This function adds the public key in PEM format and the report in the enclave and copies it in the four variables (hence the outbound `out`). 

### Attestation structure

Let's build the `Attestation` structure which will be used to add sequentially all the functions that we will be needing to get integrity and confidentiality information inside the enclave. 

First and foremost, we will start by creating a folder called common, that will have three different class definition :

```bash
$ mkdir common && cd common && touch crypto.cpp crypto.h attestation.cpp attestation.h dispatcher.cpp dispatcher.h
```

These 6 files will represent three different object classes :

- crypto class object: will contain all the cryptographic operations that will needed in the attestation operation (such as hashing, encrypting & decrypting). It's pretty much a new version of the functions that we've implemented in Part I. 

- attestation class object: will contain the methods that will retrieve the cryptographic proof needed to be sent to the host. It will be using the crypto object to use some cryptographic operations directly. 

- dispatcher class object: will handle to communication between the ecalls and the attestation class.

So, let's start by trying to add the `get_report` ecall to retrieve the enclave report. 

### The crypto class object 
The crypto class object provides functionality for encryption and hashing using the RSA algorithm and SHA-256 hash function. It uses the MbedTLS library for cryptographic operations.

??? "Explanations about the crypto class object"
    1. `Crypto::Crypto()` - Constructor method that initializes the crypto module by calling `init_mbedtls()`.

    2. `Crypto::~Crypto()` - Destructor method that frees resources allocated by the crypto module by calling `cleanup_mbedtls()`.

    3. `Crypto::init_mbedtls()` - Method that initializes the crypto module by performing the following operations:
    - Initializes the `m_entropy_context`, `m_ctr_drbg_context`, and `m_pk_context` structures from the mbedtls library.
    - Seeds the `m_ctr_drbg_context` structure with entropy using `mbedtls_ctr_drbg_seed()` function.
    - Sets up an RSA key pair of **2048-bit** with exponent **65537** using `mbedtls_rsa_gen_key()` function.
    - Writes out the public key in PEM format using `mbedtls_pk_write_pubkey_pem()` function.

    4. `Crypto::cleanup_mbedtls()` - Method that frees resources allocated by the crypto module by calling the corresponding mbedtls cleanup functions (`mbedtls_pk_free()`, `mbedtls_entropy_free()`, and `mbedtls_ctr_drbg_free()`).

    5. `Crypto::retrieve_public_key()` - Method that retrieves the public key of the enclave by copying the value of `m_public_key` to the `pem_public_key` buffer provided.

    6. `Crypto::Sha256()` - Method that computes the SHA256 hash of the provided data using the mbedtls library functions (`mbedtls_sha256_init()`, `mbedtls_sha256_starts_ret()`, `mbedtls_sha256_update_ret()`, and `mbedtls_sha256_finish_ret()`).

    7. `Crypto::Encrypt()` - Method that encrypts the provided data using the public key of another enclave. The method performs the following operations:
    - Parses the provided public key into an `mbedtls_pk_context` structure using `mbedtls_pk_parse_public_key()` function.
    - Sets the RSA padding and hash algorithm to be used for encryption.
    - Encrypts the data using `mbedtls_rsa_pkcs1_encrypt()` function with the parsed public key.
    - Sets the encrypted data size and returns `true` if successful.


### The attestation class object 

we will start by writing the class's structure and definition:
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


We are going to add an Attestation object. this **Attestation** object will implement functions for attestation: `generate_attestation_evidence` and `generate_report`.

`generate_attestation_evidence` method will generate evidence for attestation, which is a cryptographic proof of the *integrity* and *authenticity* of an enclave. The function takes in several parameters including `format_id`, `format_settings`, `data`, and `data_size`. It first hashes the input data using SHA256. Then, it initializes the attester and plugin by calling `oe_attester_initialize()`. Next, it generates custom claims for the attestation. It serializes the custom claims using `oe_serialize_custom_claims` and generates evidence based on the format selected by the attester using `oe_get_evidence`. Finally, it cleans up and returns a boolean indicating whether the function succeeded or failed.

`generate_report` generates a remote report for the given data. The SHA256 digest of the data is stored in the report_data field of the generated remote report. The function takes in parameters similar to generate_attestation_evidence. It first hashes the input data using SHA256, and then generates a remote report using oe_get_report. It sets the OE_REPORT_FLAGS_REMOTE_ATTESTATION flag to generate a remote report that can be attested remotely by an enclave running on a different platform. Finally, it cleans up and returns a boolean indicating whether the function succeeded or failed.


Before writing the ecall function, we have to make slight changes to the Makefile that must taken into account. 




### Changes to the enclave code
## Collateral 

## Verification client side 