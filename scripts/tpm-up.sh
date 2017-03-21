swtpm cuse -n tpm0 --tpmstate dir=/tmp --log file=/tmp/vtpm.log,level=20
swtpm_ioctl -i /dev/tpm0
#TPM_DEVICE=/dev/tpm0 swtpm_bios
#tcsd -f
