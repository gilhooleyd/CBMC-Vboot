
.data
.global image_body 
image_body:
.INCBIN "/home/vagrant/personal-vboot/images/FWDATA"

.global gbb_bin
gbb_bin:
.INCBIN "/home/vagrant/personal-vboot/images/gbb.blob"

.global vblock_bin
vblock_bin:
.INCBIN "/home/vagrant/personal-vboot/images/fwblock.vblock"
