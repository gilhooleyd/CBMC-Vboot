kernel.elf: custom.o boot.o print_stub.o memory_stub.o files.o
	ld -melf_i386 -T linker custom.o boot.o print_stub.o memory_stub.o files.o ../../../build/vboot_fw.a -o kernel.elf

test.elf: test.o boot.o print_stub.o
	ld -melf_i386 -T linker test.o boot.o print_stub.o -o test.elf

custom.o: ../../custom_test.c
	gcc -g -c -nostdlib -DI_BIN -ffreestanding -m32 \
		-o custom.o \
		-I ../../../host/lib/include -I ../../../firmware/include/ -I ../../../firmware/lib/cryptolib/include/ -I ../../../firmware/lib/include/  \
		../../custom_test.c 
clean:
	rm *.o
	rm test.elf

%.o : %.c
	gcc -g -c -m32 -std=gnu99 -nostdlib -ffreestanding -Wextra -Wall $< -o  $@

%.o : %.S
	as -g --32 $< -o $@
