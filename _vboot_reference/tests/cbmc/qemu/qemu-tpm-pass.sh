mkdir /tmp/myvtpm0
chown -R tss:root  /tmp/myvtpm0
swtpm_setup --tpm-state /tmp/myvtpm0  --createek
export TPM_PATH=/tmp/myvtpm0
swtpm_cuse -n vtpm0
/home/vagrant/qemu-tpm/bin/debug/native/i386-softmmu/qemu-system-i386 -s -S -kernel test.elf -tpmdev cuse-tpm,id=tpm0,path=/dev/vtpm0  -device tpm-tis,tpmdev=tpm0

