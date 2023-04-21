# Part 2 : Adding the remote attestation to our KMS code
- `openenclave/enclave.h`: contains Open Enclave SDK APIs for creating and managing Enclaves
- `openenclave/attestation/attester.h`: provides functions to perform remote attestation and to verify attestation evidence
- `openenclave/attestation/sgx/evidence.h`: defines structures and functions for attestation evidence, specifically for Intel Software Guard Extensions (SGX) Enclaves
- `openenclave/attestation/sgx/report.h`: provides functions for generating reports that attest to the current state of an SGX Enclave
- `stdlib.h`, `string.h`, `sys/socket.h`, `arpa/inet.h`, `errno.h`, `netinet/in.h`, `stdarg.h`, `stdbool.h`, `stdio.h`, `unistd.h`, `netdb.h`, `sys/types.h`, `fcntl.h`, and `sys/epoll.h`: standard C/C++ libraries and headers for working with sockets, files, and I/O operations, as well as error handling and formatting.

In addition, the code also includes two C++ files `aes_genkey.cpp` and `rsa_genkey.cpp`.

The `certificate` and `private_key` character pointers contain example values for an X.509 digital certificate and private key, respectively. These values are used as test data and should be replaced with appropriate values for a given use case.