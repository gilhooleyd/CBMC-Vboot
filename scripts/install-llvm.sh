cd ~/
wget http://releases.llvm.org/3.0/llvm-3.0.tar.gz
wget http://releases.llvm.org/3.0/clang-3.0.tar.gz
tar xvf llvm-3.0.tar.gz
tar xvf clang-3.0.tar.gz

mv clang-3.0.src llvm-3.0.src/tools/clang

cd llvm-3.0.src
mkdir build
cd build
cmake ../
make
