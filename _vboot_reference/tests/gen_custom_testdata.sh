# Load common constants and variables for tests.
. "$(dirname "$0")/common.sh"

# Load routines to generate keypairs
. "${ROOT_DIR}/scripts/keygeneration/common.sh"

# all algs
# 8 is the 4098 RSA key with 256 SHA encryption
algs="8"

# output directories
PREAMBLE_DIR="${SCRIPT_DIR}/custom_data"
DATADIR="${PREAMBLE_DIR}/keys"
V2DIR="${PREAMBLE_DIR}/vblock"

# make directories
for d in "${PREAMBLE_DIR}" "${DATADIR}" "${V2DIR}"; do
  [ -d "$d" ] || mkdir -p "$d"
done

# generate a bunch of data keys
for d in $algs; do
  make_pair "${DATADIR}/data_$d" "$d"
done

# generate a bunch of root keys
for r in $algs; do
  make_pair "${DATADIR}/root_$r" "$r"
done

# generate keyblocks using all possible combinations
for d in $algs; do
  for r in $algs; do
     make_keyblock "${DATADIR}/kb_${d}_${r}" 15 \
       "${DATADIR}/data_$d" "${DATADIR}/root_$r"
  done
done

# make a kernel key because we have to have one (crosbug.com/27142)
make_pair "${DATADIR}/kernel" 0

# and a few more dummy files just because (crosbug.com/23548)
echo "hi there" > "${DATADIR}/dummy_config.txt"
dd if=/dev/urandom bs=32768 count=1 of="${DATADIR}/dummy_bootloader.bin"

# make some fake data
dd if=/dev/urandom of="${DATADIR}/FWDATA" bs=32768 count=1
dd if=/dev/urandom of="${DATADIR}/KERNDATA" bs=32768 count=1

# Now sign the firmware and kernel data in all the possible ways using the
# pre-change tools.
for d in $algs; do
  for r in $algs; do
      echo "hello"
      vbutil_firmware --vblock "${V2DIR}/fw_${d}_${r}.vblock" \
        --keyblock "${DATADIR}/kb_${d}_${r}.keyblock" \
        --signprivate "${DATADIR}/data_${d}.vbprivk" \
        --version 1 \
        --kernelkey "${DATADIR}/kernel.vbpubk" \
        --fv "${DATADIR}/FWDATA"
        vbutil_firmware \
        --verify "${V2DIR}/fw_${d}_${r}.vblock" \
        --signpubkey "${DATADIR}/data_${d}.vbpubk" \
        --fv "${DATADIR}/FWDATA"
#     vbutil_kernel --pack "${V2DIR}/kern_${d}_${r}.vblock" \
#       --keyblock "${DATADIR}/kb_${d}_${r}.keyblock" \
#       --signprivate "${DATADIR}/data_${d}.vbprivk" \
#       --version 1 \
#       --arch arm \
#       --vmlinuz "${DATADIR}/KERNDATA" \
#       --bootloader "${DATADIR}/dummy_bootloader.bin" \
#       --config "${DATADIR}/dummy_config.txt"
  done
done
