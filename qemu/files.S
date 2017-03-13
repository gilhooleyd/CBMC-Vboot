
.data
.global image_body 
image_body:
.INCBIN "../images/FWDATA"

.global gbb_bin
gbb_bin:
.INCBIN "../images/gbb.blob"

.global vblock_bin
vblock_bin:
.INCBIN "../images/fwblock.vblock"
