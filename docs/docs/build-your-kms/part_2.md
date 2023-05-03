# Part 2 : Adding the remote attestation to our KMS code



## Some explanation and theory
Remote attestation is a security mechanism that enables a **remote entity** to verify the **integrity and authenticity** of a system or application running on another machine. This mechanism can be used to ensure that the system or application is running in a trusted environment and has not been tampered with by a malicious attacker.

***In Intel SGX***, we achieve the remote attestation by generating a enclave **report** that is then used to generate the **quote** which represents the signature of the report. 

The **quote** in Intel SGX represents a digitally signed attestation generated through a hardware and software configuration of a particular SGX enclave. It is the signature that gives proof of the integrity of the application and system (software and hardware evidence). 

It is the quote (partly) that verifies the integrity of the code inside the enclave and that it's really an application enclave running with Intel SGX protections on a trusted Intel SGX platform. 

### So how does it work? (briefly)
Architectural enclaves. More explication at https://blindai.mithrilsecurity.io/en/latest/docs/security/remote_attestation/



### Implementation on OpenEnclave

Open Enclave is an open-source project that provides a platform for building secure and confidential computing environments. In this tutorial, we will explore how to use Open Enclave to perform remote attestation.



Step 2: Create a sample attester

Next, we need to create a sample attester that will perform remote attestation. Open a new terminal window and create a new directory named "sample_attester".

```bash
mkdir sample_attester
cd sample_attester
```

Next, create a file named "attester.cpp" in the "sample_attester" directory with the following content:

```c++
#include <iostream>
#include <openenclave/host.h>
#include <openenclave/attestation/attester.h>
#include <openenclave/enclave.h>

int main(int argc, char** argv)
{
    oe_result_t result;
    oe_enclave_t* enclave = NULL;
    uint8_t* report = NULL;
    size_t report_size = 0;

    result = oe_create_sample_enclave(
        "enclave.signed.so",
        OE_ENCLAVE_TYPE_SGX,
        OE_ENCLAVE_FLAG_DEBUG,
        NULL,
        0,
        &enclave);
    if (result != OE_OK)
    {
        std::cerr << "oe_create_sample_enclave failed: " << result << std::endl;
        return 1;
    }

    result = oe_get_report(
        enclave,
        OE_REPORT_FLAGS_REMOTE,
        NULL,
        0,
        &report,
        &report_size);
    if (result != OE_OK)
    {
        std::cerr << "oe_get_report failed: " << result << std::endl;
        oe_terminate_enclave(enclave);
        return 1;
    }

    std::cout << "Attestation successful!" << std::endl;

    oe_free_report(report);
```





- `openenclave/enclave.h`: contains Open Enclave SDK APIs for creating and managing Enclaves
- `openenclave/attestation/attester.h`: provides functions to perform remote attestation and to verify attestation evidence
- `openenclave/attestation/sgx/evidence.h`: defines structures and functions for attestation evidence, specifically for Intel Software Guard Extensions (SGX) Enclaves
- `openenclave/attestation/sgx/report.h`: provides functions for generating reports that attest to the current state of an SGX Enclave
- `stdlib.h`, `string.h`, `sys/socket.h`, `arpa/inet.h`, `errno.h`, `netinet/in.h`, `stdarg.h`, `stdbool.h`, `stdio.h`, `unistd.h`, `netdb.h`, `sys/types.h`, `fcntl.h`, and `sys/epoll.h`: standard C/C++ libraries and headers for working with sockets, files, and I/O operations, as well as error handling and formatting.

In addition, the code also includes two C++ files `aes_genkey.cpp` and `rsa_genkey.cpp`.

The `certificate` and `private_key` character pointers contain example values for an X.509 digital certificate and private key, respectively. These values are used as test data and should be replaced with appropriate values for a given use case.

## Quote generation
The quote generation process begins by retrieving the report from the enclave. For that purpose, OpenEnclave SDK has an enclave **implementation** that's dedicated. Indeed, the function `oe_get_report` creates a report to be used in attestation (and specifically for our case, remote attestation). This call must be done **inside** the enclave as it is specific to the platform (and each enclave in that sense).

However, to generate the quote, we must do so from the host. Because in the quote, we also have to add to add information specific to the platform, and hence to generate the generate, the host contacts the quoting enclave with the freshly sent report from the enclave. 

## Collerateral 

## Verification client side 