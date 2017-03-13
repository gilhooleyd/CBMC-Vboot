cd ~/CBMC-Vboot/_vboot_reference
make -j4
sudo make install
mkdir ~/CBMC-Vboot/images
mkdir ~/CBMC-Vboot/qemu/build
~/CBMC-Vboot/scripts/generate-image.sh
