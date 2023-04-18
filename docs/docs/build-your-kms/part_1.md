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



## Host code 


## Enclave code 
### HTTPs server

### functions implementation

## Running and testing our code

## Next improvements