# 🛠️ Build your own Key Management System
___________________________________________________

In this series of tutorials, we will teach you how to master Confidential Computing through a playful exercise: we will **build a Key Management System** (KMS) inside a secure enclave. 

We will do it using:

- The **Open Enclave** Software Development Kit (SDK), which is a **C/C++ SDK** to build Confidential Computing apps. 
- **Intel SGX** as the main secure enclave technology, because it is the most battle-tested solution.

Hence a good knowledge would recommended in **C/C++ and cryptography**. Some hints and reminders would be added to make it more comfortable for you to read.  

A KMS is a piece of software that performs **cryptographic operations** (such as encryption and managing private keys) and is usually **embedded inside a secure hardware** component or hardware security modules (HSMs). 

Because the TEEs are also a secure environment, we can implement a working KMS inside an enclave. A remote client can benefit from having a remote key manager, without having to trust their keys directly.

***# Maybe let's put a real-life example here. Like to say how companies use it or something. Doesn't have to be long, can be 2/3 sentences but so it feels a bit more real ^^***

!!! warning "<font size="2">Don’t roll your own crypto!</font>"

	<font size="3">This tutorial is mainly for educational purposes and the KMS implemented here **it is not fully compliant with the strict security protocols ensuring the security of keys and key management** needed to be used **in production**. It will serve more as an example, like implementing an encryption algorithm yourself to better understand it. As people always say in cryptography: “don’t roll your own crypto”!</font>

***# [YASS] Redondance avec la partie de l'intro + incorrect car ce n'est pas comme implementer un algo de chiffrement mais plutôt comme réimplémenter un mécasmisme d'authentification. mais bon...***

<font size="5"><span style="font-weight: 200">
What does a KMS do? 
</span></font>
<br />
It is common for companies to store sensitive data that they must protected to keep their services available and protected. For a running web application for instance, a particular attention must be given to passwords, credit card details or other data when storing them. Usually, these issues are resolved by encryption. But the key used must also be stored securely. Which means that it must be encrypt again. And then, we have another key that we must protect and store and so on. 

This is when the purpose of a KMS comes into place. One of the features of it is to manage keys, import them, managing users and roles... in a secure and protected way. 
A KMS can perform multiple cryptographic operations. From storing private keys and certificates, to performing encryption and key rotation. 
But for our ***mini-KMS*** project, we are gonna restrict to the following:

- Symmetric encryption and decryption with AES-256
- Symmetric key generation in AES-256
- Asymmetric key generation in RSA-2048
- Key import

<font size="5"><span style="font-weight: 200">
Choosing a crypto library
</span></font>
<br />
In this project, we will be using ***Mbedtls*** for the developping the cryptographic operations above. It has the advantage of being simple to use and already rewritten for OpenEnclave. Even though it lacks some features, it has a good practical usage for our application. Mbdetls is small and does implement well in embedded systems. 

*PS: Mbedtls does not support TLS1.3 yet (experimental)*



Ready to practice? Let’s dive in and install everything we need to get started!

<br />
<br />
[Next :fontawesome-solid-forward-fast:](#){ .md-button .md-button--primary }