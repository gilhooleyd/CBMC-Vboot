#   --show-loops \
cbmc \
    -D NONDET_VARS \
    -I ./ -I ../host/lib/include/ -I ../firmware/include/ -I ../firmware/lib/cryptolib/include/ -I ../firmware/lib/include/ \
    cbmc/test_loadfirmware.c\
    ../firmware/stub/utility_stub.c ../firmware/lib/vboot_common.c ../firmware/lib/vboot_nvstorage.c ../firmware/lib/vboot_common_init.c ../firmware/lib/crc8.c ../firmware/lib/region-fw.c \
    ../firmware/lib/vboot_firmware.c \
    --unwindset Memcpy.0:1050 \
#    --bounds-check \

#    ../firmware/stub/vboot_api_stub_sf.c\
#    cbmc/test_common.c \
