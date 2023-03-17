# Introduction to Confidential Computing

<font size=”5”><span style="font-weight: 200">
A hands-on guide for beginners
</font></span>

The goal of this series is to introduce the concept of **Confidential Computing** to **developers**. 

We will highlight the current security challenges, what is Confidential Computing, and how it can be used to solve those issues.

As we believe in **learning by coding**, the rest of the articles will be oriented toward implementing an application with Confidential Computing, to better highlight how it works, its constraints, and its strengths.

To fully follow this series, **experience in C/C++ and Linux is required**. As Confidential Computing is a **hardware-based solution**, some parts will require specific machines to be run. We’ll explain which ones and how to set them up at the beginning of the tutorials.
## 🛡️ Security and privacy challenges

Data security and privacy have become crucial topics with the explosion of information creation and data sharing.

Cloud technology has been critical for this revolution to happen, because it has provided the backbone of many applications powering its growth. Software as a Service (SaaS) companies, hosted on the Cloud, provide fully managed services of great quality, scalability, and availability, under the condition that data is sent to them to handle it.

The problem? In doing so, SaaS providers have also become points of failure in terms of security by handling users' data directly. Compromission of these SaaS providers will induce the leakage of their users' data. 

This is why integrating external SaaS providers has become difficult in regulated and sensitive industries such as healthcare, finance or governments, to mitigate these risks.
## 🏥 An example: AI integration for hospitals
To better understand how data can be exposed to SaaS vendors, let’s take a real-life example.

Imagine you have a hospital wanting to use AI to help doctors be more efficient. Doctors often take voice notes in a rush, then need to transcribe them into writing for later reporting. This takes a lot of time and it could be automated with AI speech-to-text.

The hospital doesn’t have the expertise, the infrastructure or the time to do so. If they want to implement it, they would turn to external help - for example an AI provided through the SaaS of a startup. This would be beneficial because there would be no cost for onboarding or for maintenance.

The issue is that, when sending data to that startup, no technical guarantee can be provided that the confidential medical information contained in those audios will not be compromised. Data *can* be encrypted in transit, and even if the startup uses a secure Cloud, it can still be put at risk because it will be in clear in memory before being analyzed for transcription by the AI model. Therefore the hospital has to entrust their data to this startup which might not have put in place the proper security measures to ensure data confidentiality.

This key security flaw often cuts off this type of collaboration from the get go. The hospital would likely refuse to send their medical data to the SaaS startup, especially if they are at an early stage and don’t have the resources yet to invest in security. Being able to use speech-to-text could have saved many hours for the hospital’s personnel and benefit the startup, but healthcare data security is just too sensitive to take any risk. Now, what if we could allow the startup to provide their service while guaranteeing minimal risk for the hospital?
## 🔒 Providing hardware security guarantees with Confidential Computing
As you might have guessed, there is an answer and it is Confidential Computing! Our tutorial’s star technology can analyze sensitive data without ever exposing it in clear. It does so by using secure enclaves.

Secure enclaves, or **Trusted Execution Environments** (TEEs), are hardware-based secure environments. They are self-contained zones where the processor guarantees that the software running inside cannot be tampered with by the host operating system, hypervisor, and even its BIOS. The **memory isolation** provided by the hardware makes outside attempts to **read or modify fail**.

In the hospital scenario for instance, the AI startup could have put their model inside an enclave. This would have allowed the medical recordings to be analyzed by the startup without them being technically able to see the hospital’s data.!

The full process of how data is protected will be explained later in the series, so don’t worry if you don’t understand the details yet!

Confidential Computing has been gaining more and more traction and is driven today by the main hardware providers. Intel initially proposed **Intel SGX** in 2014, to secure processes, followed by AMD with **SEV SNP** to deploy confidential VMs and Nvidia will release **confidential GPUs** in 2023.

It’s therefore an exciting time to learn about Confidential Computing and we’re happy to take you with us on this journey to learn how to build confidential apps 🚀
## 🛠️ Build your own Key Management System
In this series of tutorials, we will teach you how to master Confidential Computing through a playful exercise: we will **build a Key Management System** (KMS) inside a secure enclave. 

We will do it using:

- The **Open Enclave** Software Development Kit (SDK), which is a **C/C++ SDK** to build Confidential Computing apps. 
- **Intel SGX** as the main secure enclave technology, because it is the most battle-tested solution.

A KMS is a piece of software that performs cryptographic operations (such as encryption and managing private keys) and is usually embedded inside a secure hardware component or hardware security modules (HSMs). 

Because the TEEs are also a secure environment, we can implement a working KMS inside an enclave. A remote client can benefit from having a remote key manager, without having to trust their keys directly.

As this tutorial is mainly for educational purposes, this KMS inside a secure enclave **will not have the fully-fledged security features** needed to be used **in production**. It will serve more as an example, like coding AES to better understand it. As we always say in cryptography: “don’t roll your own crypto”!

Ready to practice? Let’s dive in and install everything we need to get started!