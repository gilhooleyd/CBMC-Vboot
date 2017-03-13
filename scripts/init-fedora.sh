sudo yum -y install git

# TPMLIBS
sudo yum -y install automake autoconf bash coreutils expect libtool sed \
    libtpms libtpms-devel fuse fuse-devel glib2 glib2-devel gmp gmp-devel \
    nss-devel net-tools selinux-policy-devel gnutls gnutls-devel libtasn1 \
    libtasn1-tools libtasn1-devel rpm-build
sudo yum -y install openssl-devel tpm-tools kernel-modules-extra socat
sudo useradd tss
git clone https://github.com/stefanberger/libtpms
cd libtpms
./bootstrap.sh
./configure --prefix=/usr --with-openssl
make
sudo make install

# SWTPM
cd /vagrant
git clone https://github.com/stefanberger/swtpm
cd swtpm
./bootstrap.sh
./configure --prefix=/usr --with-openssl
make
make check -j8
sudo make install

# QEMU
yum -y install glibc-devel.i686 glibc-devel
cd /vagrant
git clone https://github.com/stefanberger/qemu-tpm
cd qemu-tpm
mkdir -p qemu-tpm/bin/debug/native
cd qemu-tpm/bin/debug/native
# Configure QEMU and start the build.
../../../configure --enable-debug  --target-list=i386-softmmu
make -j4
echo "export PATH=/vagrant/qemu-tpm/bin/debug/native/i386-softmmu/:$PATH" >> ~/.bashrc

# Boost
sudo yum -y install libfdt-devel pixman-devel SDL-devel
sudo yum -y install cmake wget gcc-c++ python-devel
sudo yum -y install boost--1.61 boost-devel--1.61 boost-jam--1.61
sudo yum -y install 'boost*'

# Z3
cd /vagrant
git clone https://github.com/Z3Prover/z3 
cd z3
python scripts/mk_make.py
cd build
make
sudo make install

# ILA 
cd /vagrant
git clone https://github.com/Bo-Yuan-Huang/ILA
cd ILA/synthesis/libcpp/
bjam
cd cbuild
echo "export PYTHONPATH=/vagrant/ILA/synthesis/libcpp/build/:$PYTHONPATH" >> ~/.bashrc
