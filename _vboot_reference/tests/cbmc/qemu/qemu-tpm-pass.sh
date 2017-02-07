mkdir /tmp/myvtpm0
chown -R tss:root  /tmp/myvtpm0
swtpm_setup --tpm-state /tmp/myvtpm0  --createek
export TPM_PATH=/tmp/myvtpm0
swtpm_cuse -n vtpm0
qemu-system-i386 -kernel test.elf -tpmdev passthrough,id=tpm0,path=/dev/vtpm0  -device tpm-tis,tpmdev=tpm0

