# SWTPM
cd ~/
git clone https://github.com/stefanberger/swtpm
cd swtpm
./bootstrap.sh
./configure --prefix=/usr --with-openssl
make
make check 
sudo make install
