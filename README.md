# Vboot Project

This project takes Chrome's Verified Boot (Vboot) process and examines its various 
security properties using formal logic.
This verification is done with a focus on the firmware/hardware boundary.

The Vboot process depends on the correct functionality of a Trusted Platform 
Module (TPM) and a SHA accelerator. Because these hardware accelerators are 
interacted with through Memory Mapped I/O (MMIO), it is difficult for normal
formal methods to capture the interface between the MMIO registers and the 
workings of the Hardware modules.

To explore this boundary I am using a [Software TPM Library](https://github.com/stefanberger/swtpm)
and passing it through to the [QEMU Hardware Emulator](http://www.qemu-project.org/).
This allows me to use the normal MMIO registers of a TPM with the original Vboot Library.

# Works Cited

My project needs numerous other github projects, and I would like to thank others
for their help. Here are the projects that will be installed 

* https://github.com/stefanberger/libtpms
* https://github.com/stefanberger/swtpm
* https://github.com/stefanberger/qemu-tpm
* https://github.com/Bo-Yuan-Huang/ILA 
* https://github.com/Z3Prover/z3
* https://chromium.googlesource.com/chromiumos/platform/vboot_reference/  

# Installation

The packages I use have only been verified on Fedora 23. For easy access I 
have been using [Vagrant](https://www.vagrantup.com/) with [VirtualBox](https://www.virtualbox.org/wiki/VirtualBox) for easy installation and compartmentalization.

Please place the `Vagrantfile` file in a directory of your choosing, run
`vagrant up` and `vagrant ssh` in the terminal.
Now that you are within the fedora image, you can run the following to install
everything

    yum install -y git
    git clone https://github.com/gilhooleyd/CBMC-Vboot
    ~/CBMC-Vboot/scripts/install-fedora.sh

Now that the installation is done, exit out of the ssh shell.
Please restart the vagrant box using the following commands
(The restart needs to happen to make sure you are running on the latest 
kernel with the correct kernel headers)

    vagrant halt
    vagrant up
    vagrant ssh

Now that you are back in the Vagrant box, you can continue with 
the rest of the installation.

    ~/CBMC-Vboot/scripts/init-fedora.sh

That command just downloaded and compiled the necessary programs, and 
everything is ready to go!

# Code Structure

This section describes an outline of each folder. 
There are more readme documents in necessary folders.

## scripts

This folder contains all of the installation scripts, as well as scripts
to generate a Vboot Firmware Image and manipulate the TPM.

## qemu

This folder contains a directory for the QEMU emulation.
There is a scaffolding framework that calls functions in the larger
Vboot library, which is stored as an archive in this directory.
This is where tests on the TPM are done.

Most of my QEMU code, including the structure for the Makefile, the linker,
and the VGA firmware was taken from my other project 
[PIOUS](https://github.com/gilhooleyd/PIOUS) which I have used to practice
Operating System Development.

## ILA

This folder contains the [ILA](https://github.com/Bo-Yuan-Huang/ILA) abstraction that I am working on for the 
Hardware/Firmware interface of the TPM and SHA modules.
