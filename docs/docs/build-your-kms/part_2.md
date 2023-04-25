# Part 2 : Adding the remote attestation to our KMS code

Remote attestation is a security mechanism that enables a remote entity to verify the integrity and authenticity of a system or application running on another machine. This mechanism can be used to ensure that a system or application is running in a trusted environment and has not been tampered with by a malicious attacker.

Open Enclave is an open-source project that provides a platform for building secure and confidential computing environments. In this tutorial, we will explore how to use Open Enclave to perform remote attestation.

Prerequisites:

1. A Linux machine with the Open Enclave SDK installed.
2. Two Linux machines with Intel SGX capable CPUs.
3. Basic knowledge of Linux and C/C++ programming.

Step 1: Create a sample enclave

First, we need to create a sample enclave that we will use to demonstrate remote attestation. Open a terminal and create a new directory named "sample_enclave".

```bash
mkdir sample_enclave
cd sample_enclave
```

Next, create a file named "enclave.edl" in the "sample_enclave" directory with the following content:

```c++
enclave {
    trusted {
        public void foo();
    };
};
```

This file defines a single function named "foo" that will be exposed by the enclave. We will implement this function in a later step.

Next, create a file named "enclave.cpp" in the "sample_enclave" directory with the following content:

```c++
#include <openenclave/enclave.h>
#include <openenclave/internal/tests.h>

void foo()
{
    OE_TEST(1);
}
```

This file contains the implementation of the "foo" function. It simply calls the "OE_TEST" macro, which will cause the function to fail if the input is not equal to 1.

Next, we need to compile the sample enclave. Run the following command in the "sample_enclave" directory:

```bash
mkdir build
cd build
cmake ..
make
```

This will generate a shared library file named "enclave.signed.so" in the "sample_enclave/build" directory.

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