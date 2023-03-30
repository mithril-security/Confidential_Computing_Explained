# (1) Installing OpenEnclave SDK (simulation and hardware modes)
______________________________________________


To build an sgx application for our minimal KMS, we will be using OpenEnclave SDK which builds enclave applications using C and C++. 
This installation guide focuses on OpenEnclave SDK running in simulation and hardware mode using Azure.

This installation guide focuses on OpenEnclave SDK running in simulation and hardware mode. Simulation mode works on any machine but it does not have all the key security features. It is good for playing around but is not intended for production. Hardware mode will have all the security features but requires specific Intel processors to be run. For real tests, you can find a machine on Azure.

[LAURA SUGGESTION FORE PREVIOUS PARAGRAPH:
This installation guide will walk you through how to set up everything you need to use the OpenEnclave SDK in both simulation mode and hardware mode. Simulation mode works on any machine but does not have all the key security features. It can be useful for running tests but is not intended for production. Hardware mode has all the Intel SGX security features but requires specific Intel processors. If you want to complete this tutorial in hardware mode, we recommend using a DCvs3 Azure VM.
]

***# Any machine or do you need some minimum processing power ? [YASS] No processing for tests***

## Pre-requisites
______________________________

### Simulation mode

- *We recommend a Linux Ubuntu distribution **18.04** or **20.04 LTS**.*


### Hardware mode

The easiest way to get started would be to create a virtual machine (**VM**) on Azure.
The VM that will be used for hardware mode in this tutorial is a **DCsv3 VM** with an **Ubuntu 20.04** OS image.

*Prerequisites:*
- *The processor must support SGX 2, with Flexible Launch Control.*
- *Linux distribution greater than or equal to 18.04 and Linux headers version greater than or equal to 5.11*
- *Azure hardware mode: Access to a DCvs3 Azure confidential VM.*

***# [LAURA COMMENT: Azure is not necessarily a pre-requisite if someone wants to follow along on their own SGX2 device though? [YASS] actually it is, we only try to give directions for azure because the KMS will only be dev for azure***

***# LAURA COMMENT: We have a guide on how to get a free trial with Azure and set up an Azure DCSV3 VM- we could always include this in this guide if you want? It could be a separate page that we link to, maybe in a `resources` folder or something? I have a copy of this file locally I can share with you] [YASS] You might be right, I'll wait for ophelie insight on this cause I am lazy and don't want to do it (maybe it's too much detail for lecturer? If he can't set up the VM on azure it's not a good begining for what will come after)***


## How to set-up Simulation mode
______________________________________

For this simulation set-up mode, we can use any dev environment. However, we highly recommend using **a Linux OS and 20.04 Ubuntu distribution** as they are the ones used in this tutorial and are also the ones that work well with OpenEnclave. 

***# [LAURA COMMENT: In pre-requisites we say you need a Linux Ubuntu >=18.04 and here we say you can use whatever environment- this is confusing! If it is not essential maybe we can say **recommended:** in the pre-requisites section.]
[YASS] I've changed the paragraph a little bit***

The simulation mode works in the same way as the hardware mode but the Intel instructions are simulated in software rather than using hardware.

*PS: Keep in mind that key security features behind confidential computing are not available in simulation mode!*

*PS: The packages used in this guide may not work for other OS or other linux distributions. The installation may also be different.*  

*When building our KMS project we will note at the beginning of each section if it is possible to follow along in simulation mode!*

### Installing OpenEnclave SDK and its dependencies

<font size="5"><span style="font-weight: 200">
Adding APT sources
</font></span>


We first need to add the necessary repos to the package manager APT to be able to install the necessary packages.
To configure the Intel and microsoft Azure APT repositories for downloading & installing Intel SGX and OpenEnclave add the following commands :

```bash
echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu focal main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -

echo "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-11 main" | sudo tee /etc/apt/sources.list.d/llvm-toolchain-focal-11.list
wget -qO - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
echo "deb [arch=amd64] https://packages.microsoft.com/ubuntu/20.04/prod focal main" | sudo tee /etc/apt/sources.list.d/msprod.list
wget -qO - https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -

sudo apt update
```

<font size="5"><span style="font-weight: 200">
Installing all the Intel and OpenEnclave packages 
</font></span>
#### 

```bash
sudo apt -y install clang-11 libssl-dev gdb libsgx-enclave-common libsgx-quote-ex libprotobuf17 libsgx-dcap-ql libsgx-dcap-ql-dev az-dcap-client open-enclave
```

Here, we've installed all the packages needed to simulate a running enclave with OpenEnclave. 
Note that we won't be able to test the important security features (for communicating remotely and securing the enclave) that a real enclave can deliver.

***# [LAURA COMMENT: Maybe it is too much, but we keep saying that with simulation we don't have the **all important security features**, but this makes me curious as to what they are- if you are able to concisely add any more details on this, it could be nice] [YASS] don't know if it's better***

You should now see that OpenEnclave was installed in the folder `/opt/openenclave/` (for more details, follow this [link](https://github.com/openenclave/openenclave/blob/master/docs/GettingStartedDocs/Linux_using_oe_sdk.md)):
***# [YASS] is the link above interesting to keep?***

```bash
$ ls /opt/openenclave
bin include lib share
 ```

To work with our current shell, we are going to add the following command to be able to run OpenEnclave tools directly from our shell without having path issues:
```bash
source /opt/openenclave/share/openenclave/openenclavec
```
To make our development journey easier, we are going to add it directly to the `.bashrc` to make it persistent in every new shell:

```bash
 echo "source /opt/openenclave/share/openenclave/openenclaverc" >> ~/.bashrc
```
## How to set-up Hardware mode

***# It get a bit unclear here. What step is for whom and what do you need to do when you're in simulation mode? Or which steps are the same for both situations? This might be the wrong title or the wrong place for it, but it was to show you an example of something we could do to clarify [YASS] I've separated the hardware from simulation, that way we see the difference more clearly***
______________________________________


### SGX drivers

***# The following two paragraphs should go =) This is a course, so we are not sending people away. They should have everything they need here. Copy pasting + aknowledging we went to Intel or another person GitHub is fine. People should never have to click on anything but the "NEXT" button at the bottom of the page ^^.***

***It's different from a product tutorial. In a product tutorial, the person comes with their own needs. We need to have all the ressources for them to be able to meet them, but we don't know what they are so it's more important that we give them an answer that drives them somewhere else than not giving it to them. With teaching, you're defining what road the student will take and they should do what you tell them, not wander around ^^. It's cool because you have more power and it's easier to keep people in your clutches than with product demo. [YASS] Done***


If you’re using a Linux kernel above version 5.11, SGX drivers should already be installed. And if your processor supports Intel SGX you can verify that it is installed and available by using the `ls` command:

```bash
~/$ ls /dev/sgx*
/dev/sgx_enclave  /dev/sgx_provision 

/dev/sgx:
enclave  provision
``` 

***# LAURA COMMENT: 1. Maybe clarify that only one of these ls commands needs to work- not both. # It's only one command 2. What about if people have SGX1 instead of SGX2? Maybe the method of checking this used in the installation guide is interesting? You could copy and paste it it's useful
https://blindai.mithrilsecurity.io/en/latest/docs/tutorials/core/installation/#on-premise-deployment # [YASS] Yep it might be a good idea, but the Azure DCsV3 are all SGX2, and the remote attestation that we are gonna do doesn't use SGX1. It'll be better to restrict to only SGX2 / Added in Post-Scriptum***

If you have a kernel version below 5.11, you should upgrade to a newer version, or you can download the drivers' binaries directly from the official intel repository by following the steps below: 
```bash

wget -c https://download.01.org/intel-sgx/latest/linux-latest/distro/ubuntu20.04-server/sgx_linux_x64_driver_1.41.bin

chmod +x sgx_linux_x64_driver_1.41.bin
./sgx_linux_x64_driver_1.41.bin
```

*PS: if the command fails with an in-kernel support error, this means the drivers are already installed.*   
*PS 2: If you want to check if you have a working version of SGX on your machine, you can checkout the steps here: https://blindai.mithrilsecurity.io/en/latest/docs/tutorials/core/installation/#on-premise-deployment.*

***# This download binary type of link is the only sort of 'go to this link' allowed ^^ [YASS] Modified***


### Installing the OpenEnclave SDK and the Intel dependencies
OpenEnclave's installation is done in three parts:
- adding the packages to the trusted apt sources list
- installing all the necessary packages and dependencies
- Testing and verifying that all the features work


***# Same as for 'simple'. Nothing is straightforward. I would either get rid of the sentence altogether or make an encouraging statement that implies now we're getting to the fun part [YASS] modified***

You can follow the instructions below.

***# Like I said a bit earlier, don't drive people out! =D Especially if you've already put all the required info down. I prefer us copy/pasting a bit of someone else's documentation while saying it's from them and that we took it from there than sending people away to go check theirs ^^ [YASS] Modified*** 

<font size="5"><span style="font-weight: 200">
Adding APT sources
</font></span>


We add the necessary repos to the package manager APT to be able to install the necessary packages.
To configure the Intel and microsoft Azure APT repositories for downloading & installing Intel SGX and OpenEnclave add the following commands:

***# Careful with caps/no caps. You're not consistent with them ^^ Microsoft. Intel. SGX. etc It's good to start getting into the habit [YASS] Can't do better haha, we have intel SGX packages and Azure packages***

```bash
echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu focal main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -

echo "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-11 main" | sudo tee /etc/apt/sources.list.d/llvm-toolchain-focal-11.list
wget -qO - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
echo "deb [arch=amd64] https://packages.microsoft.com/ubuntu/20.04/prod focal main" | sudo tee /etc/apt/sources.list.d/msprod.list
wget -qO - https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -

sudo apt update
```

<font size="5"><span style="font-weight: 200">
Installing the packages for the security features (Hardware mode with Azure DCsV3 VM)
</font></span>

***# Confused here again with who this step is for. [YASS] we're still on the hardware mode section***


***Data Center Attestation Primitives*** (DCAP) is a set of libraries and functions that provides Intel SGX attestation support for data centers and cloud providers with an attestation model leveraging **Elliptic Curve Digital Signature Algorithm (ECDSA) encryption**. 
DCAP is necessary for the remote attestation and sealing. Theses features are the ones added as security features by Intel SGX to : 
- verify remotely that a program is running in an SGX enclave. 
- store securely an enclave.

*remote attestation and sealing will be explained in more detail and implemented in the next chapters, so don't worry if you don't really grasp what it is yet.*

**However, it doesn’t work in simulation mode**. We will restrict using **remote attestation to our Azure example**. 
The DCAP drivers are normally already installed on the Azure machine.

*PS: If you are working on your own SGX-ready machine and not the DCvs3 Azure VM, the DCAP installation process may be different. please refer to the official documentation and the OpenEnclave DCAP installation.* 	

***# [LAURA COMMENT: Is the PS meaning: If you are not using the recommended the DCvs3 Azure VM, the installation process will be different? If so, maybe we can say:
PS: If you are working on your own SGX-ready machine and not the DCvs3 Azure VM, the DCAP installation process may be different. please refer to the official documentation and the OpenEnclave DCAP installation.]***
***# At this point we still don't know what an attestation is really and I think we should clarify that at some point (with a box for example or something, saying we'll go over it more in details later... But we need more info on attestations. We can't just namedrop them.) 
[YASS] I've changed to paragraph to make an quick intro without revealing too much and by saying that it will be seen after***

If you're working on an *ubuntu 20.04 with a Linux kernel version of 5.15-azure or above*, you should already have the required drivers, otherwise, some driver DCAP installation might be required. 

```bash
$ uname -r
5.15.0-1031-azure
```

To install the drivers follow these instructions :
```bash

sudo apt -y install dkms
wget https://download.01.org/intel-sgx/sgx-linux/2.19/distro/ubuntu20.04-server/sgx_linux_x64_driver_1.41.bin
chmod +x sgx_linux_x64_driver.bin
sudo ./sgx_linux_x64_driver.bin
```

***# [LAURA COMMENT: I guess this section shouldn't usually apply since if people use our recommended Azure dcv3s VM? So maybe we don't really even need it- or we can make it a bit smaller and just say: If you are using a Azure DCvs3 VM, you should already have the required DCAP drivers, but if you are not, you can install them here: ## [YASS] No, as we saw with when we had the issue with the icelake,when we deployed different VMs all the DCsv3 VMs are not the same]***

<font size="5"><span style="font-weight: 200">
Installing the Intel and OpenEnclave packages
</font></span>


```bash
sudo apt -y install clang-11 libssl-dev gdb libsgx-enclave-common libsgx-quote-ex libprotobuf17 libsgx-dcap-ql libsgx-dcap-ql-dev az-dcap-client open-enclave
```

Here, we install all the packages that we need to perform the following : 

- Loading and initialization of an enclave memory image.
- Implement handling for enclave exception conditions.
- Implement requests for platform services.
- Provisioning of keys for DCAP attestation. 
- Tools for generating enclave and signing it. 

You should now see that OpenEnclave was installed in the folder `/opt/openenclave/` (for more detail follow this [link](https://github.com/openenclave/openenclave/blob/master/docs/GettingStartedDocs/Linux_using_oe_sdk.md)):

```bash
$ ls /opt/openenclave
bin include lib share
 ```
To work with our current shell, we are going to add the following command to be able to run OpenEnclave tools directly from our shell without having path issues:
```bash
source /opt/openenclave/share/openenclave/openenclavec
```
To make our development journey easier, we are going to add it directly to the `.bashrc` to make it persistent in every new shell:

```bash
 echo "source /opt/openenclave/share/openenclave/openenclaverc" >> ~/.bashrc
```



***# You might kill me for asking this question haha. Buuut would it make sense and simplify things a lot to run this tuto only in simulation mode to start with?***
***Like: "first we'll do the KMS in simulation mode, then we'll do it on hardware mode?" Or "first we'll do the KMS in simulation mode, then we'll do something else in hardware mode"? "*** 
***Or maybe, other option, do another very little project in simulation mode that we would put before the KMS project? Totally me thinking aloud - but I'm curious to know the answer to that.***
***Part of why I'm wondering this at this point (I have not read the other two parts yet), is that if my question is not possible or not relevant, it might be because it's missing a little part in this text explaining better why we're going through both at the same time instead of just going with Simulation first (easy and for playing) and then testing what we did in Hardware mode.***
***[YASS] Well, on that we have four parts on the KMS, and the first part we are gonna do it for simulation also (and maybe a little bit of the third part), so for me it will show all that we can do with the simulation (which is really limited), we can do explanations on the first part for the simulation part while adding how we will do it, but I don't think it's really necessary to do a whole other example***
