# ILA 
cd ~/
git clone https://github.com/Bo-Yuan-Huang/ILA
cd ILA/synthesis/libcpp/
bjam
cd cbuild
echo 'export PYTHONPATH=/home/vagrant/ILA/synthesis/libcpp/build/:$PYTHONPATH' >> ~/.bashrc
