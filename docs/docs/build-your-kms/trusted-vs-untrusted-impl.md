# (3) Implementing the trusted/untrusted interfaces in SGX with OpenEnclave

## Definition of ECall and OCall and the interfaces
As explained in the last chapter, to be able to interact to and from the enclave, we have to define two different calls :
- **Enclave Calls (ECALLs)**, gives the ability to call, from the application a pre-defined function inside the enclave.  
- **Outside Calls (OCALLs)**, makes it possible to call a pre-defined function in the application from the enclave. 

Ecalls & Ocalls works differently when the data is sent between the host and the enclave. We will not go into too much detail (atleast for now), but it is best practice to keep the amount of Ocalls as low and as controled as possible. A example of tampering, is a function that may have some read write privileges that can be altered to read inside the enclave. 

## Implementation
The ECALL and OCALL functions are implemented by defining them in a ***Enclave Definition Langage (EDL)*** file. This EDL file is then passed on a tool called **edger8r** to generate to proxy files that will be used to interact between the host and enclave. 

We define the ECALL & OCALL functions the same way we write prototypes in header files in C/C++. 

The general skeleton of the EDL file resembles the following: 
```C
enclave {
    // here we declare all the ECALL functions that will be ran inside the enclave 
    trusted {
        public return_type ecall_func(
            [param_boundary] param_type param_name
        );
        // Other ecall functions...
    };


    // And here we define all the OCALL functions needed 
    untrusted {
        public return_type ocall_func(
            [param_boundary] param_type param_name
        );
        // Other ocall functions... 
    };
};
```


## KMS example
To communicate easily with our KMS, we need to have a User Interface or maybe easier, an API. Thus, we simply need an HTTP server running inside the enclave. 
And to make more convenient for us in the next chapters, we will be attempting running an HTTPS server between the enclave and the outside. 
We will be deploying a ***self-signed HTTPS server***. However, keep in mind that, in a production, this must not implemented this way. 
Through this HTTPS gateway, we'll be defined multiple endpoints necessary for interacting with our KMS. 


### Quick reminder on TLS & HTTPS
*Transport Layer Security (TLS)* is a communication securing protocol. It ensures that the communicate between two peers is secured. Its three main properties are:
- Server authentification. 
- confidentiality of the exchanged data. 
- Integrity of the exchanged data. 

One way to achieve that, is by using a *Public Key Infrastructure X.509 (PKIX)*. Using this certificate mechanism, a client can verify the server's identity. The certificate is based on the *X.509* which is a standard format for representing *public key certificates*. 

!!! 
    A public key infrastructure relies on asymmetric encryption to perform authentication by verifying the identity through certification. 
    *PS: we sign the certificate using the private key, and we verify using the public key*


Combining HTTP with a encryption layer such as TLS (which give us HTTPS) makes it thus possible to communicate data safely through HTTP.  

### Https server via an Ecall
To achieve our HTTPS server, we are going to implement a first Enclave Call (Ecall).
This Ecall will set up the HTTPS server and will run it. 
because we're running a self-signed HTTPS server, we will have first to pass on a private key and an associated certificate to the ecall that will be used inside the enclave.

!!! warning
    Remember that this must not be used as is in production mode. It's only intended as an example for running a local HTTPS server. We will be reviewing how to improve the security in the next chapters. 

We'll also be adding two other arguments, one will be for specifying the connection port and another to keep the connection alive. 

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

!!! 
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

