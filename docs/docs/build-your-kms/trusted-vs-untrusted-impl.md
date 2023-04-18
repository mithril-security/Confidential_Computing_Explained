# Implementing a baby HTTPS server
___________________________________________________

As explained in the last chapter, we have to define two different calls to be able to interact with the enclave:

- **Enclave Calls (ECALLs)** allow the application to call a pre-defined function inside the enclave.  
- **Outside Calls (OCALLs)** allow the enclave to call a pre-defined function in the application. 

Ecalls and Ocalls work differently. We will not go into too much detail explaining how (at least for now), but it is best practice to **keep** the amount of **Ocalls** as **low** and as **controlled** as possible. A misuse of an external function on a Ocall can leak or write enclave data if not properly managed.

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
## A self-signed HTTPS server

Let's get coding! 

To communicate with our KMS, we first need to have a user interface or a programming interface (API). To do so, we'll implement a **self-signed HTTPS server** running inside the enclave. We'll define multiple endpoints which will be necessary for interacting with our KMS.

In this chapter, we'll start by coding a **simplified version** of it, which **will be unsafe**. But as we go through the chapters of the course, we'll **improve** that code to make it more robust against attacks - all the way **until** we it is ready for a **realistic** scenario! 

___________________________________________
### TLS: the S in HTTPS

To communicate data safely through HTTP, you need an encryption layer such a the **Transport Layer Security** or **TLS**. When you combine them, HTTP becomes HTTPS because the TLS ensures that the communication between two peers is secured.

The three main properties of TLS are:

- Server authentification. 
- Confidentiality of the exchanged data. 
- Integrity of the exchanged data. 

One of the way to implement these properties is by using a **Public Key Infrastructure X.509 (PKIX)**. The PKI certificate mechanism allows clients to verify the server's identity, and the certificate is based on the **X.509** format, which is a standard for representing **public key certificates**. 

!!! info "Key pairs"

	A public key infrastructure relies on **asymmetric encryption** to perform **authentication**, by verifying the identity through certification. This means there are two keys: the **private key**, which is used **to sign** the certificate, and the **public key**, which is used **to verify** that the signature is the right one. 

___________________________________
## Implementing an Ecall

Our first step will be to implement an **Ecall** that **will set up the HTTPS server and run it**. 

Our HTTPS server is self-signed, which means that we will have to pass fours arguments to the ECall: 

+ **a private key**,
+ an **associated certificate** that will be used inside the enclave,
+ the **connection port**,
+ and a boolean specifying to **keep the connection alive**. 

Our EDL file can look like the following:

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

Let's begin by explaining the first import lines. We've imported three different edl files. 

The OpenEnclave edl files, depending on the use might be called in two different paths. If it's not specific to the platform, the path for the EDL becomes `openenclave/edl/<name>.edl`, otherwise, the path must indicate the platform such as `openenclave/edl/sgx/<name>.edl`. 

The first import is `syscall.edl` which encompass all of the syscalls supported (which contain also sockets, time, ioctl...). 
The second one, `platform.edl` imports all the other edl files specific to Intel SGX (We will see more about it in the next chapters). 

Next comes the trusted section where we are writing our ecall. We define it as `set_up_server` with four arguments as precendtly explained :

- a string that we only need to read representing the server's port (boundary `[in]`) 
- a string that we only need to read representing the private key (boundary `[in]`) and the size associated with.
- a string that we only need to read representing the certificate (boundary `[in]`) and the size associated with. 
- a boolean to keep the server up. 

!!! note
    Keep in mind that we'll be adding other functions to this file. 
    But we'll stick to this one for now as it's the only one that we'll be using on PART 1 of this tutorial. 

#### Passing the EDL file through the edger8r tool

We talked in the implementation section above that we use ***edger8r*** to generate proxy files that serves as the way to communicate back and forth from the enclave to the host.  

to generate those files we can run the following command : 

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

It will generate for example, the following files : 

```
kms_args.h  kms_t.c  kms_t.h (trusted) 
kms_u.c  kms_u.h (untrusted)
```

And these what we call the proxy files that define all the ecall/ocall imported and those that we write on the EDL file. 



In the next chapter, we will begin to write the HTTPS server and our KMS, and launching finally the enclave with it. We then will be trying to make some requests to test our KMS. And finally, we'll overview what we still lack and what can we improve to make it more safe and robust. 
