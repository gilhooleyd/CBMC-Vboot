gdb \
    -ex "target remote localhost:1234" \
    -ex "file build/kernel.elf"
