
OUT="/home/vagrant/CBMC-Vboot/images"
# Load chrome scripts
. "/home/vagrant/CBMC-Vboot/_vboot_reference/scripts/keygeneration/common.sh"

# make data key

make_pair "${OUT}/data" 8
make_pair "${OUT}/root" 8

make_keyblock "${OUT}/keyblock" 15 "${OUT}/data" "${OUT}/root"

make_pair "${OUT}/kernel" 0

dd if=/dev/urandom of="${OUT}/FWDATA" bs=32768 count=1

vbutil_firmware --vblock "${OUT}/fwblock.vblock"\
    --keyblock "${OUT}/keyblock.keyblock" \
    --signprivate "${OUT}/data.vbprivk" \
    --version 1 \
    --kernelkey "${OUT}/kernel.vbpubk" \
    --fv "${OUT}/FWDATA"

vbutil_firmware --verify ${OUT}/fwblock.vblock --fv ${OUT}/FWDATA --signpubkey ${OUT}/root.vbpubk
gbb_utility -c 0x100,0x1064,0x03DE80,0x1064 ${OUT}/gbb.blob
gbb_utility -s --rootkey     ${OUT}/root.vbprivk ${OUT}/gbb.blob
gbb_utility -s --recoverykey ${OUT}/root.vbprivk ${OUT}/gbb.blob
