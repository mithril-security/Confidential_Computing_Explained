# 🛠️ Build your own Key Management System
___________________________________________________

In this series of tutorials, we will teach you how to master Confidential Computing through a playful exercise: we will **build a Key Management System** (KMS) inside a secure enclave. 

We will do it using:

- The **Open Enclave** Software Development Kit (SDK), which is a **C/C++ SDK** to build Confidential Computing apps. 
- **Intel SGX** as the main secure enclave technology, because it is the most battle-tested solution.

A relatively good knowledge in **C/C++ and cryptography** is recommended. We will be adding some reminders to make it more comfortable for you to read, but it might be a bit difficult to intake low-level coding language, cryptography and confidential computing all at once. 

______________________

## What's a KMS?

A **KMS** is a piece of software that performs **cryptographic operations** (such as encryption and managing private keys). It is usually **embedded inside a secure hardware** component or inside **hardware security** modules (also referred to as HSMs). 

Because Trusted Execution Environments, or TEEs, are a secure environment, we can implement a working KMS inside an enclave. A remote client can benefit from having a remote key manager, without having to trust their keys directly.

!!! warning "<font size="2">Don’t roll your own crypto!</font>"

	<font size="3">This tutorial is mainly for educational purposes and the KMS implemented here **is not fully compliant with the strict security protocols needed to ensure that the security levels of keys and key management** are high enough to be used **in production**. It will serve more as an example, like implementing an encryption algorithm yourself to better understand it. As people always say in cryptography: “don’t roll your own crypto”!</font>


______________________

## What does a KMS do? 

It is common for companies to **store** sensitive data and need to **protect** it. 

For instance, let's take a running web application: a particular attention must be given to passwords and credit card details when storing them. Usually, these issues are resolved by **encryption**. 

For the encryption to be secure, **the key to decrypt must be stored securely**, and that key must **be encrypted by another key** to protect it. 

This key chain can quickly become quite complicated, especially in company setting. But at the root of the concept, there is always going to be a **master key** that we must securely store, and it **cannot be** done by **simply encrypting** it.

This where a **KMS** comes in handy. One of its features is to **manage keys**: it will import them, manage the users and the roles, etc. It will do so **in a secure and protected way**, completely **isolated** from the services that use it. That is because KMSs can perform multiple cryptographic operations. They can store private keys and certificates, perform encryption and key rotation... 

______________________________________

## Building a mini-KMS

In our **mini-KMS** project, we will restrict the operations to make it easier for you and implement:

- Symmetric encryption and decryption with AES-256
- Symmetric key generation in AES-256
- Asymmetric key generation in RSA-2048
- Key import

To build them, we will be using the open-source C cryptographic library [**Mbedtls**](https://github.com/Mbed-TLS/mbedtls). It is both simple to use and already written for Open Enclave. It's also small and implements well in embedded systems. Mbedtls does lack some features, but its advantages made up for it for the purposes of this tutorial.

!!! Info

	Mbedtls does not support TLS1.3 yet *(experimental)*.


Ready to practice? Let’s dive in and install everything we need to get started!

<br />
<br />
[Next :fontawesome-solid-forward-fast:](./installation.md){ .md-button .md-button--primary }