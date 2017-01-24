vbutil_firmware --vblock "fw.vblock" \
    --keyblock "firmware.keyblock" \
    --signprivate "root_key.vbprivk" \
    --version 1 \
    --kernelkey "kernel_subkey.vbpubk" \
    --fv "FW_VOL"

vbutil_firmware \
    --verify "fw.vblock" \
    --signpubkey "root_key.vbpubk" \
    --fv "FW_VOL" 
