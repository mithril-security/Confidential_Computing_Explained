# Installing OpenEnclave SDK (simulation and hardware modes)

## Context
To build an sgx application for our minimal KMS, we will be using OpenEnclave SDK which builds enclave applications using C and C++. 
This installation guide focuses on OpenEnclave SDK running on simulation and hardware mode using Azure.

The easiest way to get started would be to provision a virtual machine on Azure.
Provision a DCsv3 with an Ubuntu 20.04 OS image.


Prerequisites:
- On Hardware installation, the processor must support SGX 2, with Flexible Launch Control. 
- simulation mode: A linux ubuntu >18.04 distribution.
- azure hardware mode: Access to a DCvs3 azure confidential VM. Linux distribution > 18.04 and linux headers > 5.11
	
 

## SGX Drivers
If you are running on your own server or machine, you can check out if it supports SGX [here](https://www.intel.com/content/www/us/en/support/articles/000028173/processors.html).

or check support directly by following the instructions [here](https://github.com/openenclave/openenclave/blob/master/docs/GettingStartedDocs/SGXSupportLevel.md).

If you’re using a linux kernel above version 5.11, SGX drivers should already be installed and if your processor supports Intel SGX you can verify that it is installed and available by using a simple `ls`:

```bash
~/$ ls /dev/sgx*
/dev/sgx_enclave  /dev/sgx_provision 

/dev/sgx:
enclave  provision
``` 

If you have a kernel version below 5.11, you should upgrade to a newer version, or you can download the drivers' binaries directly from the official intel repository https://download.01.org/intel-sgx/latest/linux-latest/distro/.

## Installing the OpenEnclave SDK 
OpenEnclave's installation is fairly straightforward.
You can follow the instructions [here](https://github.com/openenclave/openenclave/blob/master/docs/GettingStartedDocs/install_oe_sdk-Ubuntu_20.04.md)
### Adding APT sources
Configuring the Intel and microsoft APT repositories:
```bash
echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu focal main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -

echo "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-11 main" | sudo tee /etc/apt/sources.list.d/llvm-toolchain-focal-11.list
wget -qO - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
echo "deb [arch=amd64] https://packages.microsoft.com/ubuntu/20.04/prod focal main" | sudo tee /etc/apt/sources.list.d/msprod.list
wget -qO - https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -

sudo apt update
```
### Installing the DCAP for the remote attestation (Hardware mode with Azure)
DCAP (for DataCenterAttestationPrimitives)  provides intel SGX with an attestation model leveraging ECDSA encryption. DCAP is necessary for the remote attestation. However, it doesn’t work in simulation mode. We will restrict using the remote attestation only for our Azure example. 
The DCAP drivers are normally already installed on the azure machine. 
PS: If you are working on a bare-metal machine or your own that has SGX in it, the DCAP installation is different. please refer to the official documentation and the OpenEnclave DCAP installation. 	



So when is the PSW needed ? 
- The PSW is the code that implements the following:
- Loading of an enclave memory image.
- Initialization of an enclave image.
- Execute ECALL's into the enclave image and any OCALL's resulting from those ECALL's.
- Implement handling for enclave exception conditions.
- Implement requests for platform services.
- Mediate provisioning of EPID private keys.
- Support transmission and receipt of remote attestation quotes.


