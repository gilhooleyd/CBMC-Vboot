all: test_tpm

test_tpm: test_tpm.c
	gcc -o test_tpm test_tpm.c ../../../build/vboot_fw.a -I ../../../firmware/include/ -I ../../../firmware/lib/include
