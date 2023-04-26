# Implementing a baby HTTPS server
___________________________________________________

As explained in the last chapter, we have to define two different calls to be able to interact with the enclave:

- **Enclave Calls (ECALLs)** allow the application to call a pre-defined function inside the enclave.  
- **Outside Calls (OCALLs)** allow the enclave to call a pre-defined function in the application. 

Ecalls and Ocalls work differently. We will not go into too much detail explaining how (at least for now), but it is best practice to **keep** the amount of **Ocalls** as **low** and as **controlled** as possible. A misuse of an external function on a Ocall can leak or write enclave data if not properly implemented.

___________________________________________________
## EDL and proxy files

To implement the Ecall and Ocall, we'll need to define them in a **Enclave Definition Langage (EDL)** file. 

Then we'll pass this EDL file to a tool called **edger8r**. We'll use it to generate **proxy files**, which will handle interactions between the host and the enclave. 

We define the Ecall and Ocall functions the same way we write prototypes in header files in C/C++. 

The general skeleton of an EDL file resembles the following: 

```C
enclave {
    // Here we declare all the Ecall functions that will be ran inside the enclave 
    trusted {
        public return_type ecall_func(
            [param_boundary] param_type param_name
        );
        // Other Ecall functions...
    };


    // Here we define all the Ocall functions needed 
    untrusted {
        public return_type ocall_func(
            [param_boundary] param_type param_name
        );
        // Other Ocall functions... 
    };
};
```

________________________________
## HTTPS: the KMS API

Let's get coding! 

To communicate with our KMS, we first need to have a user interface or a programming interface (API). To do so, we'll implement a **self-signed HTTPS server** running inside the enclave. We'll define multiple endpoints which will be necessary for interacting with our KMS.

In this chapter, we'll start by coding a **simplified version** of it, which **will be unsafe**. But as we go through the chapters of the course, we'll **improve** that code to make it more robust against attacks - all the way **until** we it is ready for a **realistic** scenario! 

___________________________________________
## TLS: the 'S' in 'HTTPS'

To communicate data safely through HTTP, you need an encryption layer such as the **Transport Layer Security** or **TLS**. When you combine them, HTTP becomes HTTPS because the TLS ensures that the communication between two peers is secured.

The three main properties of TLS are:

- Server authentification. 
- Confidentiality of the exchanged data. 
- Integrity of the exchanged data. 

One of the way to implement these properties is by using a **Public Key Infrastructure X.509 (PKIX)**. The PKI certificate mechanism allows clients to verify the server's identity, and the certificate is based on the **X.509** format, which is a standard for representing **public key certificates**. 

!!! info PKI

	A public key infrastructure relies on **asymmetric encryption** to perform **authentication**, by verifying the identity through certification. This means there are two keys: the **private key**, which is used **to sign** the certificate, and the **public key**, which is used **to verify** that the signature is the right one. 

___________________________________
## Set up and run the server

To **set up the HTTPS server and run it**, we'll **implement an Enclave Call (Ecall)**! 

Our HTTPS server is self-signed, which means that we will have to pass four arguments to the Ecall: 

+ **a private key**,
+ an **associated certificate** that will be used inside the enclave,
+ the **connection port**,
+ and a boolean specifying to **keep the connection alive**. 

Create a `kms.edl` file and copy/paste, write in the following code block:

```c
// kms.edl
enclave {
    from "openenclave/edl/syscall.edl" import *; // syscalls
    from "platform.edl" import *; // platform specific sgx calls


    trusted {
        public int set_up_server(
            [in, string] const char* port, 
            [in, string] const char* private_key, size_t len_private_key,
            [in, string] const char* certificate, size_t len_certificate, 
            bool keep_server_up
        );
    };

    // untrusted {

    // };
};
```

### Importing the EDL files

The first lines **import** different **EDL files**: 

```c
enclave {
    from "openenclave/edl/syscall.edl" import *; // syscalls
    from "platform.edl" import *; // platform specific sgx calls
```

+ `syscall.edl` which encompass all of the syscalls supported (which contain also sockets, time, ioctl...). 
+ `platform.edl` imports all the other EDL files specific to Intel SGX (which we'll dive in more in the next chapters). 

### Implementing the Ecall

Next comes the **trusted section**, where we write our Ecall:

```c
	trusted {
			public int set_up_server(
				[in, string] const char* port, 
				[in, string] const char* private_key, size_t len_private_key,
				[in, string] const char* certificate, size_t len_certificate, 
				bool keep_server_up
			);
		};
```  

We define the Ecall as `set_up_server` with the four arguments we detailled earlier : 

+ a **string** representing the server's **port** (boundary `[in]`). 
+ a **string** representing the **private key** (boundary `[in]`) and the size associated with it.
+ a **string** representing the **certificate** (boundary `[in]`) and the size associated with it. 
+ a **boolean** to **keep the server up**. 

The boundary is `in` because we only need to read them.

!!! note

    We'll be adding other functions to this file in the future! But this is the only one we'll need for the first part of this course, so we'll go over them at a later stage.

### Proxy files and the `edger8r` tool

It's now time to use the **edger8r** tool to generate the proxy files! They'll be the way to communicate back and forth between the enclave and the host. 

To generate those files we can run the following commands. You can try them for demonstration and exploration purposes - but as you'll see in the next chapter, you won't have to type in those commands manually because they'll be in a Makefile. 

```bash
# for the trusted part
oeedger8r kms.edl --trusted \
    --search-path /opt/openenclave/share/pkgconfig/../../include \
    --search-path /opt/openenclave/share/pkgconfig/../../include/openenclave/edl/sgx 

# for the untrusted part 
oeedger8r kms.edl --untrusted \
    --search-path /opt/openenclave/share/pkgconfig/../../include \
    --search-path /opt/openenclave/share/pkgconfig/../../include/openenclave/edl/sgx 

```

In our case, we'll call the proxy files generated by those commands with the following names:

```bash
kms_args.h  kms_t.c  kms_t.h (trusted) 
kms_u.c  kms_u.h (untrusted)
```

Those proxy files define all the imported Ecall and Ocall, as well as those that we'll write in the EDL file.   


In the next chapter, we will begin to write the HTTPS server and our KMS. We'll also launch our first enclave! 
<br />
<br />
[Part 1 :fontawesome-solid-forward-fast:](./part1_running_kms_enclave.md){ .md-button .md-button--primary }
