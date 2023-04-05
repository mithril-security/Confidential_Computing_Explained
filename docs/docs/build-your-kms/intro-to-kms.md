# 🛠️ Build your own Key Management System
___________________________________________________

In this series of tutorials, we will teach you how to master Confidential Computing through a playful exercise: we will **build a Key Management System** (KMS) inside a secure enclave. 

We will do it using:

- The **Open Enclave** Software Development Kit (SDK), which is a **C/C++ SDK** to build Confidential Computing apps. 
- **Intel SGX** as the main secure enclave technology, because it is the most battle-tested solution.

A relatively good knowledge in **C/C++ and cryptography** is recommended. We will be adding some reminders to make it more comfortable for you to read, but it might be a bit difficult to intake low-level coding langage, cryptography and confidential computing all at once. 

______________________

## What's a KMS?

A **KMS** is a piece of software that performs **cryptographic operations** (such as encryption and managing private keys) and is usually **embedded inside a secure hardware** component or hardware security modules (also refered to as HSMs). 

Because TEEs are a secure environment, we can implement a working KMS inside an enclave. A remote client can benefit from having a remote key manager, without having to trust their keys directly.

!!! warning "<font size="2">Don’t roll your own crypto!</font>"

	<font size="3">This tutorial is mainly for educational purposes and the KMS implemented here **it is not fully compliant with the strict security protocols ensuring the security of keys and key management** needed to be used **in production**. It will serve more as an example, like implementing an encryption algorithm yourself to better understand it. As people always say in cryptography: “don’t roll your own crypto”!</font>


______________________

## What does a KMS do? 

It is common for companies to store sensitive data that they must protect to keep their services available and protected. For a running web application for instance, a particular attention must be given to passwords, credit card details or other data when storing them. Usually, these issues are resolved by encryption. But the key used must also be stored securely. Which means that it must be encrypt again. And then, we have another key that we must protect and store and so on. 

This where a KMS comes in handy. One of its features is to manage keys: it will import them, manage the users and the roles (etc.) in a secure and protected way! That is because KMS can perform multiple cryptographic operations. They can store private keys and certificates, perform encryption and key rotation... 

______________________________________

## Building a mini-KMS

In our **mini-KMS** project, we will restrict the operations to make it easier for you and implement:

- Symmetric encryption and decryption with AES-256
- Symmetric key generation in AES-256
- Asymmetric key generation in RSA-2048
- Key import

To build them, we will be using the open-source C cryptographic library [**Mbedtls**](https://github.com/Mbed-TLS/mbedtls). It is both simple to use and already written for OpenEnclave. It's also small and implements well in embedded systems. Mbedtls does lack some features, but its advantages made up for it for the purposes of this tutorial.

!!! Info

	Mbedtls does not support TLS1.3 yet *(experimental)*.


Ready to practice? Let’s dive in and install everything we need to get started!

<br />
<br />
[Next :fontawesome-solid-forward-fast:](./installation.md){ .md-button .md-button--primary }