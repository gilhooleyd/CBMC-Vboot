kernel.elf: custom.o boot.o print_stub.o memory_stub.o files.o
	ld -melf_i386 -T linker custom.o boot.o print_stub.o memory_stub.o files.o vboot_fw.a -o kernel.elf

test.elf: test.o boot.o print_stub.o memory_stub.o  tpm.o
	ld -L./ -melf_i386 -T linker test.o boot.o print_stub.o tpm.o memory_stub.o  -lvboot -o test.elf
#	ld -melf_i386 -T linker test.o boot.o print_stub.o -o test.elf

pcr.elf: tpm-pcr.o print_stub.o boot.o memory_stub.o tpm.o util.o
	ld -L./ -melf_i386 -T linker tpm-pcr.o boot.o print_stub.o tpm.o memory_stub.o util.o -lvboot -o test.elf

custom.o: ../../custom_test.c
	gcc -g -c -nostdlib -DI_BIN -ffreestanding -m32 \
		-o custom.o \
		-I ../../../host/lib/include -I ../../../firmware/include/ -I ../../../firmware/lib/cryptolib/include/ -I ../../../firmware/lib/include/  \
		../../custom_test.c 

test.o: test.c
	gcc -g -c -nostdlib -DI_BIN -ffreestanding -m32 \
		-o test.o \
		-I ../../../host/lib/include -I ../../../firmware/include/ -I ../../../firmware/lib/cryptolib/include/ -I ../../../firmware/lib/include/  \
		test.c
clean:
	rm *.o
	rm *.elf

%.o : %.c
	gcc -g -c -m32 -std=gnu99 -nostdlib -ffreestanding -Wextra -Wall \
		-I ../../../host/lib/include -I ../../../firmware/include/ -I ../../../firmware/lib/cryptolib/include/ -I ../../../firmware/lib/include/  \
		$< -o  $@

%.o : %.S
	as -g --32 $< -o $@
