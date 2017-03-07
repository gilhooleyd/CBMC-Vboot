CC_OPS     := -g -c -nostdlib -DI_BIN -ffreestanding -m32
VBOOT_BASE := ../_vboot_reference
INC_DIRS   := -I $(VBOOT_BASE)/firmware/include/ \
	-I $(VBOOT_BASE)/firmware/lib/cryptolib/include/ \
	-I $(VBOOT_BASE)/host/lib/include/ \
	-I $(VBOOT_BASE)/firmware/lib/include/  \
	-I include/

KERNEL_OBJ = custom.o boot.o print_stub.o memory_stub.o files.o
TEST_OBJ = test.o boot.o print_stub.o memory_stub.o tpm.o util.o
PCR_OBJ = tpm-pcr.o print_stub.o boot.o memory_stub.o tpm.o util.o

kernel.elf: $(KERNEL_OBJ)
	ld -melf_i386 -T linker $^ vboot_fw.a -o build/kernel.elf

test.elf: $(TEST_OBJ)
	ld -L./ -melf_i386 -T linker $^ -lvboot -o build/test.elf
#	ld -melf_i386 -T linker test.o boot.o print_stub.o -o test.elf

pcr.elf: $(PCR_OBJ)
	ld -L./ -melf_i386 -T linker $^ -lvboot -o build/test.elf
	mv *.o build/

custom.o: $(VBOOT_BASE)/tests/custom_test.c 
	gcc $(CC_OPS) \
		-o custom.o \
		$(INC_DIRS) \
		$(VBOOT_BASE)/tests/custom_test.c 

test.o: test.c
	gcc $(CC_OPS) \
		-o test.o \
		$(INC_DIRS) \
		test.c
clean:
	rm build/*.o
	rm build/*.elf

%.o : %.c
	gcc $(CC_OPS) \
		$(INC_DIRS) \
		$< -o  $@

%.o : %.S
	as -g --32 $< -o $@
