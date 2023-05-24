# Installation
______________________________________________

To build an SGX application for our minimal KMS, we will be using **Open Enclave SDK**, which builds enclave applications using C and C++. 

We will walk you through how to set up everything you need to use it in both **simulation mode** and **hardware mode**. 

- **Simulation mode** works on **any machine** but **does not have all the key security features**. For example, you won't be able to communicate remotely and secure the enclave. Simulation mode can be useful for running **tests** but is not intended for production. 
- **Hardware mode** has **all the Intel SGX security features** but **requires specific Intel processors**. If you want to complete this tutorial in hardware mode, we **recommend** using a **DCvs3 Azure VM**.

## Pre-requisites
______________________________

=== "Simulation mode"

	- We highly recommend a **Linux Ubuntu** distribution **18.04** or **20.04 LTS**.

		For the simulation set-up mode, you can technically use any development environment, but we'll be using the Linux distribution in this course. If you use a different setup, the packages might **not work** and the installation could be **different**. It might also work less well with Open Enclave SDK.

	!!! warning

		<font size="3">
		Keep in mind that **key security features** behind confidential computing are **not available in simulation** mode!
		
		The simulation mode works in the same way as the hardware mode. The difference is that the **Intel instructions are simulated in software** rather than using hardware. This is why many security features are not available - since they are inseparable from the hardware.
		</font>

	When building our KMS project we will tell you at the beginning of each section if it is possible to follow along in simulation mode! 


=== "Hardware mode"

	- The processor must support **SGX 2**, with **Flexible Launch Control**.
	- **Linux** distribution greater than or equal to **18.04** and **Linux headers version** greater than or equal to **5.11**.

	The easiest way to get started is to create a virtual machine (**VM**) on Azure, because it has all those requirements. The VM that will be used for hardware mode in this tutorial is a **DCsv3 VM** with an **Ubuntu 20.04** OS image. 

    ??? "How to get a DCsv3 VM with a free Azure trial"
        You can check how to deploy a DCsv3 VM from Azure by following the steps till the ssh connection at [https://blindai.mithrilsecurity.io/en/latest/docs/tutorials/core/installation/#deployment-on-azure-dcsv3](https://blindai.mithrilsecurity.io/en/latest/docs/tutorials/core/installation/#deployment-on-azure-dcsv3). 

## Set-up your mode
______________

=== "Simulation mode"

	Open Enclave's installation is done in two steps in simulation mode:

	- **Adding** the packages to the **trusted apt sources** list.
	- **Installing** all the necessary **packages** and **dependencies**.

	<h3>Adding APT sources</h3>
	____________________________

	To install the Open Enclave SDK packages and its dependencies, we'll first need to add the necessary repos to the **package manager APT**.

	Use the following commands to configure the Intel and Microsoft Azure APT repositories for downloading and installing Intel SGX and Open Enclave:

	```bash
	echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu focal main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
	wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -

	echo "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-11 main" | sudo tee /etc/apt/sources.list.d/llvm-toolchain-focal-11.list
	wget -qO - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
	echo "deb [arch=amd64] https://packages.microsoft.com/ubuntu/20.04/prod focal main" | sudo tee /etc/apt/sources.list.d/msprod.list
	wget -qO - https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -

	sudo apt update
	```

	<h3>Intel & Open Enclave packages</h3>
	_____________________________________

	Then, we'll install all the packages needed to simulate a running enclave with Open Enclave. 

	```bash
	sudo apt -y install clang-11 libssl-dev gdb libsgx-enclave-common libsgx-quote-ex libprotobuf17 libsgx-dcap-ql libsgx-dcap-ql-dev az-dcap-client open-enclave
	```

	You should now see that Open Enclave was installed in the folder `/opt/openenclave/`. 

	```bash
	$ ls /opt/openenclave
	bin include lib share
	```

	!!! abstract "Want to know more?"

		To start exploring the Open Enclave SDK once you have it installed, you can go read this [README](https://github.com/openenclave/openenclave/blob/master/docs/GettingStartedDocs/Linux_using_oe_sdk.md)!

	To run Open Enclave tools directly from our shell without having path issues, we are going to add the following command:

	```bash
	source /opt/openenclave/share/openenclave/openenclavec
	```

	Then we'll make our development journey easier, by adding it directly to the `.bashrc`. It will make this change persistent in every new shell:

	```bash
	echo "source /opt/openenclave/share/openenclave/openenclaverc" >> ~/.bashrc
	```


=== "Hardware mode"

	<h3>SGX drivers</h3>
	_________________________________

	If you’re using a Linux kernel **above** version **5.11**, SGX drivers should already be installed.

	If your processor supports Intel SGX2, you can verify that the drivers are installed and available by using the `ls` command:

	```bash
	~/$ ls /dev/sgx*
	/dev/sgx_enclave  /dev/sgx_provision 

	/dev/sgx:
	enclave  provision
	``` 

	If you find the drivers named "enclave" and "provision" (or "sgx_enclave" and "sgx_provision") in `/dev/`, you are good to go!

	If you have a kernel version **below 5.11**, you should upgrade to a newer version, or you can download the drivers' binaries directly from the official intel repository by following the steps below: 

	```bash
	wget -c https://download.01.org/intel-sgx/latest/linux-latest/distro/ubuntu20.04-server/sgx_linux_x64_driver_1.41.bin

	chmod +x sgx_linux_x64_driver_1.41.bin
	./sgx_linux_x64_driver_1.41.bin
	```  

	!!! info

		+ If the command fails with an `in-kernel support` error, this means the drivers are already installed in `/dev/sgx/`. 


	<h3>Open Enclave SDK & Intel dependencies</h3>
	_________________________________________

	Open Enclave's installation is done in three steps:

	- **Adding** the packages to the **trusted apt sources** list.
	- **Installing** all the necessary **packages** and **dependencies**.
	- **Testing** and **verifying** that all the **features work**.

	<br />
	<font size="4"><span style="font-weight: 400">
	Adding APT sources
	</span></font>

	To install the Open Enclave SDK packages and its dependencies, we'll first need to add the necessary repos to the **package manager APT**.

	Use the following commands to configure the Intel and Microsoft Azure APT repositories for downloading and installing Intel SGX and Open Enclave:

	```bash
	echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu focal main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
	wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -

	echo "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-11 main" | sudo tee /etc/apt/sources.list.d/llvm-toolchain-focal-11.list
	wget -qO - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
	echo "deb [arch=amd64] https://packages.microsoft.com/ubuntu/20.04/prod focal main" | sudo tee /etc/apt/sources.list.d/msprod.list
	wget -qO - https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -

	sudo apt update
	```

	<br />
	<font size="4"><span style="font-weight: 400">
	Packages for the security features
	</span></font>

	**Data Center Attestation Primitives** (**DCAP**) is a set of libraries and functions that will allow us to **verify remotely that a program is running** in an SGX enclave and **store the enclave** securely. These two major features are called **attestation** and **sealing**, but we'll go over them more in details later in the course. 

	DCAP drivers are normally already installed on the Azure machine, if you are working on an **ubuntu 20.04 with a Linux kernel version of 5.15-azure or above**. You can check which version you're using with the following command.

	```bash
	$ uname -r
	5.15.0-1031-azure
	```

	If you are not, some driver DCAP installation might be required:

	```bash
	sudo apt -y install dkms
	wget https://download.01.org/intel-sgx/sgx-linux/2.19/distro/ubuntu20.04-server/sgx_linux_x64_driver_1.41.bin
	chmod +x sgx_linux_x64_driver.bin
	sudo ./sgx_linux_x64_driver.bin
	```

	<br />
	<font size="4"><span style="font-weight: 400">
	Installing the Intel and Open Enclave packages
	</span></font>

	Here, we'll install all the packages that we need to perform the following tasks.  

	- Loading and initializing an enclave memory image.
	- Implement handling for enclave exception conditions.
	- Implement requests for platform services.
	- Provisioning keys for DCAP attestation. 
	- Generating an enclave and signing it. 

	```bash
	sudo apt -y install clang-11 libssl-dev gdb libsgx-enclave-common libsgx-quote-ex libprotobuf17 libsgx-dcap-ql libsgx-dcap-ql-dev az-dcap-client open-enclave
	```

	You should now see that Open Enclave was installed in the folder `/opt/openenclave/`.

	```bash
	$ ls /opt/openenclave
	bin include lib share
	```

	!!! abstract "Want to know more?"

		To start exploring the Open Enclave SDK once you have it installed, you can go read this [README](https://github.com/openenclave/openenclave/blob/master/docs/GettingStartedDocs/Linux_using_oe_sdk.md)!


	To run Open Enclave tools directly from our shell without having path issues, we are going to add the following command:

	```bash
	source /opt/openenclave/share/openenclave/openenclavec
	```

	Then we'll make our development journey easier, by adding it directly to the `.bashrc`. It will make this change persistent in every new shell:

	```bash
	echo "source /opt/openenclave/share/openenclave/openenclaverc" >> ~/.bashrc
	```

<br />
<br />
[Next :fontawesome-solid-forward-fast:](./trusted-vs-untrusted.md){ .md-button .md-button--primary }