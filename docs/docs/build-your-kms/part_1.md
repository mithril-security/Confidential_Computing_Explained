# (4) Part 1: Running the KMS inside the enclave
!!! 
    This part can be ran in simulation mode. 

## Setting up the makefiles and compilation logic 
The structure of the code will be seperated in two.

- One will be running in the host and will the enclave. 
- And one will be the code running inside the enclave. 

A skeleton of the `makefiles` was written to make it smoother to build and run the code that we will be adding. 

### Cloning the project 
Let's begin by cloning the project :
```bash
$ git clone https://github.com/mithril-security/Confidential_Computing_Explained.git
$ cd mini_kms
```
In the `mini_kms` directory, you'll find a `skeleton` directory, and other folders (`part_1`, `part_2`...) where each one correspond to a part on the tutorial. 

!!! Note
    *That way, is you are already familiar with how to do things, and you want to test a particular features you can directly jump into it !*

    You can copy the folder onto another directory or work on it as is. 

### Exploring the skeleton folder 
The tree for the skeleton code is the following: 
```c
skeleton/
├── Makefile            // The Makefile that runs the Enclave & host Makefiles
├── README.md
├── config.mk           // Used by the Makefile to detect and set the compiler installed on the machine
├── enclave
│   ├── Makefile        // Enclave Makefile
│   ├── enclave.cpp     // Where the enclave code will be written
│   ├── kms.conf        // Sets up information related to the execution of the enclave
│   └── trace.h
├── host
│   ├── Makefile        // Host Makefile
│   └── host.cpp        // Where the host code will be written
└── kms.edl             // Our EDL file

2 directories, 10 files
```


### In the enclave
The Makefile specifies rules for building an Enclave program using the Open Enclave SDK (Software Development Kit) and signing the program using the oesign tool.

To make it easier for us to develop our HTTPS server we will be using a webserver library called ***Mongoose***. from the skeleton folder put the following commands: 
```bash
$ cd skeleton/ && git clone https://github.com/cesanta/mongoose.git
```
***Mongoose*** is a relatively minimal C embedded web server.



The Makefile defines several variables such as `CRYPTO_LDFLAGS`, `CFLAGS`, `LDFLAGS`, `INCDIR`, `CC`, and `CXX`, which are used in the build process. The `all` target depends on the `build`, `keys`, and `sign` targets, which respectively compile the Enclave program, generate cryptographic keys, and sign the program using the keys. The `clean` target removes all generated files.

The Makefile uses the **oeedger8r** tool to generate interface code from the `kms.edl` file, which defines the Enclave interface. It then compiles the Enclave program and links it with the Open Enclave SDK libraries and the cryptographic libraries specified by the `CRYPTO_LDFLAGS` variable. Finally, the program is signed using the private key generated by the 'keys' target and the kms.conf configuration file.

To add socket, network and syscall support to the Enclave, rewritten libraries for OpenEnclave must be linked at compilation time. Such as `-loelibcxx` for C++ components, `-loehostsock`, for socket support, `-loehostresolver` provides DNS resolution services and `-loehostepoll`, for `epoll` system call implementation must be added to the linking phase. These components are necessary to be able to run the web server and communicate through it. 

```makefile

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
LDFLAGS=$(shell pkg-config oeenclave-$(COMPILER) --libs)
INCDIR=$(shell pkg-config oeenclave-$(COMPILER) --variable=includedir)

all:
	$(MAKE) build
	$(MAKE) keys
	$(MAKE) sign

build:
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../kms.edl --trusted \
		--search-path $(INCDIR) \
		--search-path $(INCDIR)/openenclave/edl/sgx
	$(CXX) -g -c $(CFLAGS) -DOE_API_VERSION=2 enclave.cpp -o enclave.o
	$(CC) -g -c $(CFLAGS) -DOE_API_VERSION=2 kms_t.c -o kms_t.o
	$(CC) -g -c $(CFLAGS) -DOE_API_VERSION=2 ../mongoose/mongoose.c -lmbedtls -lmbedcrypto -lmbedx509 -D MG_ENABLE_MBEDTLS=1 -o mongoose.o
	$(CXX) -o enclave kms_t.o mongoose.o enclave.o -D MG_ENABLE_MBEDTLS=1 -loelibcxx -loehostsock -loehostresolver -loehostepoll $(LDFLAGS) $(CRYPTO_LDFLAGS) 

sign:
	oesign sign -e enclave -c kms.conf -k private.pem

clean:
	rm -f mongoose.o enclave.o enclave enclave.signed private.pem public.pem kms_t.o kms_t.h kms_t.c kms_args.h

keys:
	openssl genrsa -out private.pem -3 3072
	openssl rsa -in private.pem -pubout -out public.pem
```
### In the host 
The Makefile for building the untrusted host program. The `build` target uses **oeedger8r** to generate interface code from the `kms.edl` file, which defines the interface between the untrusted host program and the trusted Enclave program. It then compiles the host program and links it with the Open Enclave SDK libraries specified by the `LDFLAGS` variable.

The `clean` target removes all generated files.

The Makefile defines the `CFLAGS`, `LDFLAGS`, and `INCDIR` variables to obtain compiler and linker options from the Open Enclave SDK. It compiles `host.cpp` and `kms_u.c` into object files and links them into a single executable file named `kms_host`.

The `build` target does not build or sign the Enclave program. 
```makefile
# miniKMS

include ../config.mk

CFLAGS=$(shell pkg-config oehost-$(COMPILER) --cflags)
LDFLAGS=$(shell pkg-config oehost-$(COMPILER) --libs)
INCDIR=$(shell pkg-config oehost-$(COMPILER) --variable=includedir)

build:
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../kms.edl --untrusted \
		--search-path $(INCDIR) \
		--search-path $(INCDIR)/openenclave/edl/sgx
	$(CXX) -g -c $(CFLAGS) host.cpp
	$(CC) -g -c $(CFLAGS) kms_u.c
	$(CXX) -o kms_host kms_u.o host.o $(LDFLAGS)

clean:
	rm -f kms_host host.o kms_u.o kms_u.c kms_u.h kms_args.h
```
When run, the `kms_host` executable launches the Enclave and runs it. 

So let's start by writing the Host code that will launch the Enclave and start the server via the Ecall. 
## Host code 

We begin by adding the necessary header files for the host, including `stdio.h` for standard I/O operations, `openenclave/host.h` for creating enclaves, `sys/stat.h` and `sys/types.h` for working with file permissions, `fstream` and `iostream` for working with file streams, and string for working with strings.
It also includes the header file `"kms_u.h"`.



```C++
#include <stdio.h>
#include <openenclave/host.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <string>

#include "kms_u.h"

using namespace std;
```


This function checks if the `--simulation` option was passed as a command line argument and returns a boolean value indicating whether or not to run the program in simulation mode.


```C++
bool check_simulate_opt(int* argc, const char* argv[])
{
    for (int i=0; i<*argc; i++)
    {
        if (strcmp(argv[i], "--simulation"))
        {
            cout << "Running on simulation mode" << endl;
            memmove(&argv[i], &argv[i+1], (*argc - i)* sizeof(char *));
            (*argc)--;
            return true; 
        }
    }
    return false;
}
```


This function creates an enclave by loading the enclave binary image from the specified `enclave_path` and initializing the enclave with the specified `flags`.

```C++
oe_enclave_t* create_enclave(const char* enclave_path, uint32_t flags)
{
    oe_enclave_t* enclave = NULL;

    printf("[Host]: Enclave path %s\n", enclave_path);
    oe_result_t result = oe_create_kms_enclave(
        enclave_path, 
        OE_ENCLAVE_TYPE_AUTO, 
        flags, 
        NULL, 
        0, 
        &enclave);

    if (result != OE_OK)
    {
        printf(
            "[Host]: Enclave creation failed at enclave init : %s\n", oe_result_str(result)
        ); 
    }
    else {
        printf(
            "[Host]: Enclave created Successfully.\n"
        );
    }
    return enclave;
}
```



This is the main function that is executed when the program is run. It first declares and initializes some variables, including a flag for debugging, a pointer to an enclave, and variables to specify the port and whether to keep the server up. It then creates the enclave using the `create_enclave` function and the provided enclave path and flags. If the enclave creation fails, the program jumps to the `exit` label and terminates the program. Otherwise, it sets up the http server using the `set_up_server` function with the enclave, port, and server keep-up variables. If the server setup fails, it also jumps to the `exit` label and terminates the program. Finally, it terminates the enclave and returns a status code.

```C++
    oe_result_t result;
    int ret = 1;
    uint32_t flags = OE_ENCLAVE_FLAG_DEBUG;
    oe_enclave_t *enclave = NULL;
    const char* server_port_untrusted = "9001";
    bool keep_server_up = false; 
    
    cout << "[Host]: entering main" << endl;

    // if (check_simulate_opt(&argc, argv))
    // {
    //     flags |= OE_ENCLAVE_FLAG_SIMULATE;
    // }

    // if (argc != 2)
    // {
    //     cout << "Usage" << argv[0] << "enclave_image_path [ --simulation ]"  << endl; 
    //     goto exit;
    // }

    // Enclave creation 
    enclave = create_enclave(argv[1], flags);
    if (enclave == NULL)
    {
        goto exit;
    }

    printf("[Host]: Setting up the http server.\n");

    ret = set_up_server(enclave, &ret, server_port_untrusted, keep_server_up);
    if (ret!=0)
    {
        printf("[Host]: set_up_server failed.\n");
        goto exit;
    }

exit: 
    cout << "[Host]: terminate the enclave" << endl;
    cout << "[Host]: running with exit successfully." << endl;
    oe_terminate_enclave(enclave);
    return ret; 
```


## Enclave code 
This is a C/C++ code snippet that includes several header files and defines two character pointers `certificate` and `private_key`. 

The enclave code will be the core of the KMS. 
Let's start by importing the necessary headers to run our enclave: 
- `openenclave/enclave.h`: contains Open Enclave SDK APIs for creating and managing Enclaves
- `stdlib.h`, `string.h`, `sys/socket.h`, `arpa/inet.h`, `errno.h`, `netinet/in.h`, `stdarg.h`, `stdbool.h`, `stdio.h`, `unistd.h`, `netdb.h`, `sys/types.h`, `fcntl.h`, and `sys/epoll.h`: standard C/C++ libraries and headers for working with sockets, files, and I/O operations, as well as error handling and formatting.

In addition, the code also includes C++ files `aes_genkey.cpp` and `rsa_genkey.cpp` that we will implement next for our KMS functions.

let's start by creating those files :
```bash 
$ touch aes_genkey.cpp rsa_genkey.cpp
```

On our file `enclave.cpp` we can go ahead and includes all of these headers:


The `certificate` and `private_key` character pointers contain example values for an X.509 digital certificate and private key, respectively. These values are used as test data and should be replaced with appropriate values for a given use case.

```C++
#include <openenclave/enclave.h>
#include <openenclave/attestation/attester.h>
#include <openenclave/attestation/sgx/evidence.h>
#include <openenclave/attestation/sgx/report.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>


#include <sys/epoll.h>
#include<fcntl.h>

#include "../mongoose/mongoose.h"



#include "kms_t.h"
#include "trace.h"

#include "generation/aes_genkey.cpp"
#include "generation/rsa_genkey.cpp"
```

### Loading the OpenEnclave modules
While it is necessary to add the network components at compilation time, we still need to load them. The function `load_oe_modules` serves that purpose by calling the loading function for each feature:
```C++
oe_result_t load_oe_modules()
{
    oe_result_t result = OE_FAILURE;
    // host resolver
    if ((result = oe_load_module_host_resolver()) != OE_OK)
    {
        printf(
            "oe_load_module_host_resolver failed with %s\n",
            oe_result_str(result));
        goto exit;
    }

    // sockets 
    if ((result = oe_load_module_host_socket_interface()) != OE_OK)
    {
        printf(
            "oe_load_module_host_socket_interface failed with %s\n",
            oe_result_str(result));
        goto exit;
    }
    // epoll 
    if ((result = oe_load_module_host_epoll())!=OE_OK) {
                printf(
            "oe_load_module_host_epoll failed with %s\n",
            oe_result_str(result));
        goto exit;
    }
exit:
    return result;
}
```


### HTTPs server
We first declare the certificate and private key as constant `char`.

### functions implementation

## Running and testing our code

## Next improvements