sudo yum -y install git

# TPMLIBS
sudo yum -y install automake autoconf bash coreutils expect libtool sed \
    libtpms libtpms-devel fuse fuse-devel glib2 glib2-devel gmp gmp-devel \
    nss-devel net-tools selinux-policy-devel gnutls gnutls-devel libtasn1 \
    libtasn1-tools libtasn1-devel rpm-build
sudo yum -y install openssl-devel tpm-tools kernel-modules-extra socat

# SWTPM
sudo yum -y install glibc-devel.i686 glibc-devel

# Boost
sudo yum -y install libfdt-devel pixman-devel SDL-devel
sudo yum -y install cmake wget gcc-c++ python-devel
sudo yum -y install boost--1.61 boost-devel--1.61 boost-jam--1.61
sudo yum -y install 'boost*'

# Vboot
sudo yum install -y libyaml-devel lzma-devel lzma glibc-static uuid-devel libuuid-devel xorg-x11-apps xorg-x11-xauth

