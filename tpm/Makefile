VBDIR=../_vboot_reference
all: test_tpm

test_tpm: test_tpm.c
	gcc -o test_tpm test_tpm.c ./vboot_fw.a \
		-I $(VBDIR)/firmware/include/ -I $(VBDIR)/firmware/lib/include

vboot:
	cd ../_vboot_reference && make fwlib DEBUG=1
	cp ../_vboot_reference/build/vboot_fw.a ../tpm/

clean:
	rm test_tpm
