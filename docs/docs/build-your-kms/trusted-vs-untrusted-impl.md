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


## KMS interfaces

## KMS calls  
### 