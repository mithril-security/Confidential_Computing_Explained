# Trusted VS Untrusted
__________________________________________________

When we **run a program**, we usually do so in an **environment**. It could be a Linux OS, a virtual machine or a Docker container. Securing that environnement is **as critical** as securing our code.

Easier said than done though! All the programs that run aside of ours, like the different standard libraries, the different softwares and even OS's kernel, can extend the attack surface. They could interact with our program and tamper with it.

Cue to one of the major principles in computer security... We must always try to **reduce the attack surface** and consequently, keep the **computing base minimal**!

{==

Achieving this principle is at the core of Confidential Computing, by defining an environment which is **highly isolated**, but can, **in certain conditions**, communicate with the outside world. 

==}

The **Trusted Computing Base** or **TCB** level defines this minimal environment. Remember this acronym because we'll be using it a lot! 
________________________________

## What is a Trusted Computing Base (TCB)? 

The TCB refers to the **system components** where the **security** of that system is **established and maintained**. 

When we talk about a "**trusted**" computing base, we don't necessarily mean that the system is secure, but that these components are **critical for the system’s security**. They are the **root of trust**, because the system assumes they are secure enough to be trusted. 

We must, after all, start trusting **somewhere**. This is actually what defines a TCB and why it must be as minimal as possible. 

To better understand what is a TCB, let's take an example: **a web application is hosted on a private instance**. This is a common architecture for an instance hosted by a cloud provider. 

The web app's TCB level is defined in this order: 

- First, we need to trust **the code of the application** itself. 
- Then the **operating system**, because it does all the necessary low-level operations. 
- The **hypervisor** which runs the guest's operating system and manages all the memory through the different guest.
- And, finally, the **hardware** which runs all these components.

Each one of these layers presents a consequent surface attack, and by adding it to the TCB, we must keep in mind that the security of each layer must be tested. But it can be easier said than done as each layer contains several vulnerable entry points that we are not aware of, which makes it harder to secure. 

![Current tcb level](../assets/tcb_level_current.png)

***# [OPHELIE] Could you change the red to color-blind-palette-red': '#D81B60' for colorblindness purposes? Or do something a bit more solid to play with contrast?***

***Also for the text, maybe more something like this?***

***"The highlight in red shows the current TCB level for most of the usual application that we run."***

***"Note that the application is not necessarily isolated from the rest of the applications in this scenario - something confidential computing ensures."***

_______________________________

## How to reduce the TCB?

We want the surface of attack to be as restricted as possible, so our main goal is **always** to reduce the TCB of our infrastructure as much as possible. 

With Confidential Computing, the idea is to strip down that TCB to its **essentials**. 

!!! success "There are two essentials:"

	<font size="3">
	- We **own the application** so we should trust its **code**. 

	- We also trust the **hardware** that has features of isolation and encryption.</font>

You might have noticed we completely removed the need to trust the **operating system** and the **hypervisor** in this scenario. 

This is a major improvement because the operating system's rich functionalities make it very hard to define proper security policy models. And we don't have to worry about the control a compromised hypervisor could have on our application. 

This base is the root of enclaves or **Trusted Execution Environment** (TEEs). 

___________________________

## Intel SGX's TCB

Intel SGX, one of the first TEE that was developed, only has the following TCB in its threat model: 

- The application code (enclave)
- The hardware

But how can we be sure that code is doing as expected if it's running in an environnement in complete isolation, especially as a remote user?

This is where the **remote attestation** comes in! It allows the enclave to attest that the application is running on real verified hardware.

We'll go over the details of this complex mechanism in the next chapters. For now, all you need to know is that it verifies that the application is **run in a protected zone** and that its **integrity** (protecting data and code against tampering) and **confidentiality**(protecting data and code against leak or extraction) are maintained.  

![SGX tcb level](../assets/tcb_level_sgx.png)

***# [OPHELIE] same but with color-blind-palette-green': '#004D40'***

Even the **BIOS** and the **firmware aren't trusted** in **Intel SGX**! Other current TEEs don't go as far, because it makes it a bit more difficult to set up the enclave. But as a result, Intel SGX's TCB is really minimal since only parts of the hardware are trusted.

### Process isolation and memory encryption

To satisfy this minimal TCB, Intel SGX relies on **process isolation**, to render **interaction** between the **host and the enclave impossible**, unless under certain conditions (for remote attestation purposes).

It also uses **memory encryption**, so that **every call** to the **dynamic memory access** (DRAM) is **encrypted**. Even if the data contained in an enclave was leaked (memory dump), it would be useless to the attacker because it can't be read.

The enclave still needs to interact with the operating system once in a while (again, for attestation purposes as we'll explain in future chapters). When implementing an Intel SGX app, we'll need to define two different calls: 

- A call from the host to the enclave (called **Enclave Call**)
- A call from the enclave to the host (called **Outside Call**)

Enough theory, let's get coding! 

In the next chapter, we'll walk you through the definition and implementation of these calls.

<br />
<br />
[Next Chapter Under Construction :fontawesome-solid-hammer:](../../index.md){ .md-button .md-button--primary }