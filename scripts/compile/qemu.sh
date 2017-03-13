# QEMU
cd ~/
git clone https://github.com/stefanberger/qemu-tpm
cd qemu-tpm
mkdir -p bin/debug/native
cd bin/debug/native
# Configure QEMU and start the build.
../../../configure --enable-debug  --target-list=i386-softmmu
make -j4
echo 'export PATH=/home/vagrant/qemu-tpm/bin/debug/native/i386-softmmu/:$PATH' >> ~/.bashrc
