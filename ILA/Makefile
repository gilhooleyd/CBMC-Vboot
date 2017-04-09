
tpm_sim.c: tpm_uniq.o
	~/llvm-3.0.src/build/bin/llc -march=c < tpm_uniq.o > tpm_sim.c

%.o : %.cpp
	~/llvm-3.0.src/build/bin/clang -I/usr/include/x86_64-linux-gnu/c++/4.8 -emit-llvm -c $< -o $@
