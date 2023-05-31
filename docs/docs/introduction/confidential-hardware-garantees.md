# 🔒 What is Confidential Computing?
___________________________________________________

As you might have guessed, there is an answer to the security issues mentioned in the last chapter: **Confidential Computing**! 

Our tutorial’s star technology can analyze sensitive data **without ever exposing it in clear**. It does so by using **Trusted Execution Environments** (TEEs), hardware-based secure environments also sometimes referred to as **secure enclaves**. 

They are self-contained zones where the processor guarantees that the software running inside cannot be tampered with by the host operating system, hypervisor, and even its BIOS. The **memory isolation** provided by the hardware makes outside attempts to **read or modify fail**.

In the previous hospital scenario for instance, the AI startup could put their speech-to-text model inside an enclave. This would allow them to analyze the medical note recorded by the doctors, but without being able to technically see the hospital’s data!

The full process of how data is protected will be explained later in the series, so don’t worry if you don’t understand the details yet!

______________________________________________________

<font size="5"><span style="font-weight: 200">
A technology on the rise
</font></span>

Confidential Computing has been gaining more and more traction and is driven today by the main hardware providers. Intel initially proposed **Intel SGX** in 2014, to secure processes. They were followed by AMD with **SEV SNP** to deploy confidential VMs. And Nvidia will release **confidential GPUs** in 2023.

It’s an exciting time to learn about Confidential Computing! We’re happy to take you with us on this journey to learn **how to build confidential apps** 🚀

<br />
<br />
[Build your KMS :fontawesome-solid-door-open:](../build-your-kms/intro-to-kms.md){ .md-button .md-button--primary }