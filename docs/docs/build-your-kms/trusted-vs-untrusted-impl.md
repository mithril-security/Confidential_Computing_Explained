# (3) Implementing the trusted/untrusted interfaces in SGX with OpenEnclave

## Definition of ECall and OCall and the interfaces
As explained in the last chapter, to be able to interact to and from the enclave, we have to define two different calls :
- **Enclave Calls (ECALLs)**, gives the ability to call, from the application a pre-defined function inside the enclave.  
- **Outside Calls (OCALLs)**, makes it possible to call a pre-defined function in the application from the enclave. 

Ecalls & Ocalls works differently when the data is sent between the host and the enclave. We will not go into too much detail, but as we don't want to  
## Implementation
The ECALL and OCALL functions are implemented by defining them in a Enclave Definition Langage (EDL) file. This EDL file is then passed on a tool called **edger8r**. 

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

### Quick reminder on TLS & HTTPS
*Transport Layer Security (TLS)* is a communication securing protocol. It ensures that the communicate between two peers is secured. Its three main properties are:
- Server authentification. 
- confidentiality of the exchanged data. 
- Integrity of the exchanged data. 

One way to achieve that, is by using a *Public Key Infrastructure X.509 (PKIX)*.
Combining HTTP with a encryption layer such as TLS (which give us HTTPS) makes it thus possible to communicate data safely through HTTP.  

### Https server via an Ecall

 


#### server ecall
To set up the server, we'll need to deliver to the enclave certain information. 
- The port it will be working on. 
- The association of a private key and certificate to establish a secure communication with a third party. 


#### EDL bounds

