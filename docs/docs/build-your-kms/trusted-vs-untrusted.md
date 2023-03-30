# (2) Trusted and untrusted concepts in confidential computing
__________________________________________________


When we run a program, we usually do so in a certain environment. It could be Linux machine, a virtual machine or a Docker container. Securing that environnement is as critical as securing our code.

Easier said than done though! All the programs that run aside of ours - the different standard libraries, the different softwares and even OS's kernel - can extend the attack surface. They could interact with our program and tamper with it.

Cue to one of the major principles in computer security: 

{==

**We must always try to reduce the attack surface and consequently, keep the computing base minimal.**

==}

By trying to achieve this principle, the concept around confidential computing is to define an environment which is **highly isolated**, but can, **in certain conditions**, communicate with the outside world. 

***# [YASS] It's the other way around, the tcb level is (defines) the minimal environment*** 

The trusted computing base (TCB) at a certain program level will be this minimal environment. Remember this acronym because we'll be using it a lot! 

Actually, here's a bit of theory on the topic before we get coding.
 
## What is a Trusted Computing Base (TCB)? 

The TCB refers to the system components where the security of that system is established and maintained. 

Here, when we talk about trust, we don't necessarily mean that the system is secure, but that these components are critical for the system’s security. ***# A BIT UNCLEAR. Do you mean that those system components are the root of trust and a place where the system just assumes that things are secure? It's missing a little something for clarity. [YASS] well the first idea was the right one. We don't assume that they are secure. But they are enough secure to be trusted as the root of trust. Because we must start trusting something somewhere (that's what actually defines the tcb)*** This is why the TCB must be as minimal as possible. 

[LAURA COMMENT: I get what you mean, but I agree, I don't think this is very clear- I would try re-wording this]

***# For example, I rewrote parts of the following two paragraphs*** 

Let's take an example: a web application is hosted on a private instance. This is a common current architecture of an instance hosted by a cloud provider. 

The web app's TCB level is defined in this order: first, we need to trust **the code of the application** itself. Then the **operating system**, because it does all the necessary low-level operations. After comes the **hypervisor** which runs the guest's operating system and manages all the memory through the different guest, and, finally the **hardware**, that runs all these components.

***# Why put only an explanation on the second one? [YASS] added other infos***



Each one of these layers presents a consequent surface attack, and by adding it to the TCB, we must keep in mind that the security of each layer must be tested. But it can be easier said than done as each layer contains several vulnerable entry points that we are not aware of, which makes it harder to make secure. 

***# This doesn't show that much haha. It's barely feeding the surface of my curiousness ^^ Maybe here we need a bit more info - not necessarily a lot but what are thos intricate mechanisms, etc? The sentence is a bit shallow. [YASS] is it better?***

![Current tcb level](../assets/tcb_level_current.png)

## How to reduce the TCB?

We want the surface of attack to be as restricted as possible, so our main goal is *always* to reduce the TCB of our infrastructure as much as possible. 

With Confidential Computing, the idea is to strip down that TCB to its **essentials**. 

!!! success "There are two essentials:"

	<font size="3">

	- We **own the application** so we should trust its **code**. 

	- We also trust the **hardware** that has features of isolation and encryption.</font>

This base is the root of **Trusted Execution Environment** or **TEE**. Remember that acronym too- it's probably the most important one to know in relation to confidential computing!

You might have noticed we completely removed the need to trust the **operating system** and the **hypervisor** in this scenario. 

This is a major improvement because the operating system's rich functionalities make it very hard to define proper security policy models. And we don't have to worry about the control a compromised hypervisor could have on our application. 

## Intel SGX's TCB

Intel SGX, one of the first TEE that was developed, only has the following TCB in its threat model: 

- The application code (enclave)
- The hardware

But how can we be sure that code is doing as expected if it's running in an environnement in complete isolation, especially as a remote user? This is where the **remote attestation** comes in. It allows the enclave to attest that the application is running on real verified hardware.

We'll go over the details of this complex mechanism in the next chapters. For now, all you need to know is that it verifies that the application is **run in a protected zone** and that its **integrity *(Protecting data & code against tampering)* and confidentiality *(Protecting data & code against leak or extraction)*** are maintained.  

![SGX tcb level](../assets/tcb_level_sgx.png)

Even the BIOS and the firmware aren't trusted in Intel SGX! Other current TEEs don't go as far - because it makes it a bit more difficult to set up the enclave. But as a result, Intel SGX's TCB is really minimal since only parts of the hardware are trusted.

### Process isolation and memory encryption


To satisfy this minimal TCB, Intel SGX relies on **process isolation**, to render **interaction** between the **host and the enclave impossible**, unless under certain conditions. (For attestation purposes, as we will see in the next chapter).

It also uses **memory encryption**, so that **every call** to the **dynamic memory access** (DRAM) is **encrypted**. This means that even if the data contained in an enclave was leaked (memory dump), it would be useless to the attacker because it can't be read.

But as we said earlier, it still needs to interact with the operating system once in a while. When implementing an Intel SGX app, we'll need to define two different calls: 

- A call from the host to the enclave (called **Enclave Call**)
- A call from the enclave to the host (called **Outside Call**)

In the next chapter, you'll start working on the KMS while we walk you through the definition and implementation of these calls!