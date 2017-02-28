
.data
.global image_body 
image_body:
.INCBIN "/home/vagrant/Chrome/vboot/_vboot_reference/tests/preamble_tests/data/FWDATA"

.global gbb_bin
gbb_bin:
.INCBIN "/home/vagrant/Chrome/vboot/_vboot_reference/tests/preamble_tests/gbb.blob"

.global vblock_bin
vblock_bin:
.INCBIN "/home/vagrant/Chrome/vboot/_vboot_reference/tests/preamble_tests/preamble_v2x/fw_8_8.vblock"
