# 🛡️ Security and privacy challenges
___________________________________________________

**Data security** and **privacy** have become crucial topics with the explosion of information creation and data sharing.

**Cloud technology** has been critical for this revolution to happen, because it has provided the backbone of many applications powering its growth. Software as a Service (**SaaS**) companies, hosted on the Cloud, provide fully managed services of great quality, scalability, and availability, under the condition that data is sent to them to handle it.

**The problem?** In doing so, SaaS providers have also become **points of failure** in terms of security by **handling users' data directly**. Compromission of these SaaS providers will induce the leakage of their users' data. 

This is why integrating external SaaS providers has become difficult in regulated and sensitive industries such as healthcare, finance or governments, to mitigate these risks.

______________________________________________________

<font size="5"><span style="font-weight: 200">
🏥 An example: AI integration for hospitals
</font></span>

To better understand how data can be exposed to SaaS vendors, let’s take a real-life example.

Imagine you have a hospital wanting to use AI to help doctors be more efficient. Doctors often take voice notes in a rush, then need to transcribe them into writing for later reporting. This takes a lot of time and it could be automated with AI speech-to-text.

The hospital doesn’t have the expertise, the infrastructure or the time to do so. If they want to implement it, they would turn to external help - for example an AI provided through the SaaS of a startup. This would be beneficial because there would be no cost for onboarding or for maintenance.

The issue is that, when sending data to that startup, no technical guarantee can be provided that the confidential medical information contained in those audios will not be compromised. Data *can* be encrypted in transit, and even if the startup uses a secure Cloud, it can still be put at risk because it will be in clear in memory before being analyzed for transcription by the AI model. Therefore the hospital has to entrust their data to this startup which might not have put in place the proper security measures to ensure data confidentiality.

This key security flaw often cuts off this type of collaboration from the get go. The hospital would likely refuse to send their medical data to the SaaS startup, especially if they are at an early stage and don’t have the resources yet to invest in security. Being able to use speech-to-text could have saved many hours for the hospital’s personnel and benefit the startup, but healthcare data security is just too sensitive to take any risk. Now, what if we could allow the startup to provide their service while guaranteeing minimal risk for the hospital?

<br />
<br />
[Next :fontawesome-solid-forward-fast:](./confidential-hardware-garantees.md){ .md-button .md-button--primary }
