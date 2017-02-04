test.elf: test.o boot.o print_stub.o
	ld -melf_i386 -T linker test.o boot.o print_stub.o -o test.elf

clean:
	rm *.o
	rm test.elf

%.o : %.c
	gcc -g -c -m32 -std=gnu99 -nostdlib -ffreestanding -Wextra -Wall $< -o  $@

%.o : %.S
	as -g --32 $< -o $@
