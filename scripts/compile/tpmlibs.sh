# TPMLIBS
cd ~/
sudo useradd tss
git clone https://github.com/stefanberger/libtpms
cd libtpms
./bootstrap.sh
./configure --prefix=/usr --with-openssl
make
sudo make install
