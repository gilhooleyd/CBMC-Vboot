# TPM Readme

This directory uses Chrome's Vboot with the built-in
virtual TPM. This connects to the CUSE TPM that is located
at /dev/tpm0

The output `tpm_test` needs to be run as sudo. 
If it cannot connect make sure that `tpm-up.sh` is run,
as root, from the `scripts` folder.

## Current Status

At the moment, `test_tpm` initializes the TPM, extends the 
0th PCR register, reads the first PCR register, and compares
values.

## Rebuilding Chrome

The vboot_fw.a archive is a bundle of vboot functions.
It is needed for this to be built.
If changes are made to vboot and they want to be reflected here, 
then run `make vboot` to remake the archive.
