CC_OPS     := -g -c -nostdlib -DI_BIN -ffreestanding -m32
VBOOT_BASE := ../_vboot_reference
INC_DIRS   := -I $(VBOOT_BASE)/firmware/include/ \
	-I $(VBOOT_BASE)/firmware/lib/cryptolib/include/ \
	-I $(VBOOT_BASE)/host/lib/include/ \
	-I $(VBOOT_BASE)/firmware/lib/include/  \
	-I $(VBOOT_BASE)/firmware/lib/tpm_lite/include/  \
	-I include/

KERNEL_OBJ = custom.o boot.o print_stub.o memory_stub.o files.o
TEST_OBJ = test.o boot.o print_stub.o memory_stub.o tpm.o util.o
PCR_OBJ = tpm-pcr.o print_stub.o boot.o memory_stub.o tpm.o util.o
BAD_OBJ = tpm-badcmds.o print_stub.o boot.o memory_stub.o tpm.o util.o

vboot_verif: $(KERNEL_OBJ)
	ld -L./ -melf_i386 -T linker $^ -lvboot_notpm -o build/kernel.elf
	mv *.o build/

single_cmd: $(TEST_OBJ)
	ld -L./ -melf_i386 -T linker $^ -lvboot -o build/kernel.elf
	mv *.o build/

pcr: $(PCR_OBJ)
	ld -L./ -melf_i386 -T linker $^ -lvboot -o build/kernel.elf
	mv *.o build/

badcmds: $(BAD_OBJ)
	ld -L./ -melf_i386 -T linker $^ -lvboot -o build/kernel.elf
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

vboot:
	cd ../_vboot_reference && make clean
	cd ../_vboot_reference && make fwlib FIRMWARE_ARCH=X86_QEMU
	cp ../_vboot_reference/build/vboot_fw.a ./libvboot.a

vboot_notpm:
	cd ../_vboot_reference && make clean
	cd ../_vboot_reference && make fwlib FIRMWARE_ARCH=X86_QEMU MOCK_TPM=1
	cp ../_vboot_reference/build/vboot_fw.a ./libvboot_notpm.a

