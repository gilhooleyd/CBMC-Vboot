# QEMU Readme

This directory is for the QEMU project. It runs through
Vboot inside of QEMU and tests the QEMU's TPM implementation.

In order to get this to work you must have [Stefan Berger's QEMU
implemenation](https://github.com/stefanberger/qemu-tpm).

## Running for the First Time

You will need to run `make vboot` and `make vboot_notpm` if you 
are running this for the first time.
These commands build vboot with and without the tpm and these 
archive libraries are needed for the final binaries.

You will also need to run `scripts/generate_image.sh` to create
the vboot structures necessary for `vboot_verif`.

## Make Commands

* vboot_verif: This runs through the whole vboot image verification
process. At the moment there is no TPM included
* single_cmd: Runs only a single TPM command
* pcr: Initializes the TPM and reads + extends a PCR register

The make commands all output a final binary as `build/kernel.elf`.
To run this you will need to run `sudo qemu-tpm-pass.sh` which 
runs QEMU with the TPM passthrough extension.

## Running GDB

To run gdb run the following commmands:

`sudo ./qemu-tpm-pass.sh D`

`./gdb-start.sh`




