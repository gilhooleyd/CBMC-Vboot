
.data
.global image_body 
image_body:
.INCBIN "../images/FWDATA"

.global gbb_bin
gbb_bin:
.INCBIN "../images/gbb.blob"

/bin/bash: indent: command not found
.INCBIN "../images/fwblock.vblock"
