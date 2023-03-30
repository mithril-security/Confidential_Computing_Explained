# 🛠️ Build your own Key Management System
___________________________________________________

In this series of tutorials, we will teach you how to master Confidential Computing through a playful exercise: we will **build a Key Management System** (KMS) inside a secure enclave. 

We will do it using:

- The **Open Enclave** Software Development Kit (SDK), which is a **C/C++ SDK** to build Confidential Computing apps. 
- **Intel SGX** as the main secure enclave technology, because it is the most battle-tested solution.

A KMS is a piece of software that performs **cryptographic operations** (such as encryption and managing private keys) and is usually **embedded inside a secure hardware** component or hardware security modules (HSMs). 

Because the TEEs are also a secure environment, we can implement a working KMS inside an enclave. A remote client can benefit from having a remote key manager, without having to trust their keys directly.

***# Maybe let's put a real-life example here. Like to say how companies use it or something. Doesn't have to be long, can be 2/3 sentences but so it feels a bit more real ^^***

!!! warning "<font size="2">Don’t roll your own crypto!</font>"

	<font size="3">This tutorial is mainly for educational purposes and the KMS implemented here **will not have the fully-fledged security features** needed to be used **in production**. It will serve more as an example, like implementing an encryption algorithm yourself to better understand it. As people always say in cryptography: “don’t roll your own crypto”!</font>

Ready to practice? Let’s dive in and install everything we need to get started!

<br />
<br />
[Next :fontawesome-solid-forward-fast:](#){ .md-button .md-button--primary }