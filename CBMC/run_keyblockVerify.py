import os

vboot_dir = "../_vboot_reference/"

includedDirs = [
        vboot_dir + "host/lib/include",
        vboot_dir + "firmware/include/",
        vboot_dir + "firmware/lib/cryptolib/include/",
        vboot_dir + "firmware/lib/include/",
        vboot_dir + "tests"
        ]

testFile = "test_keyblockVerify.c"

includedFiles = [
        # include file below to do Google's unit test assertions
#        "test_common.c",

        vboot_dir + "firmware/stub/utility_stub.c",
        vboot_dir + "firmware/lib/vboot_common.c",
        vboot_dir + "firmware/lib/vboot_nvstorage.c",
        vboot_dir + "firmware/lib/vboot_common_init.c",
        vboot_dir + "firmware/lib/crc8.c",
#        vboot_dir + "firmware/lib/region-fw.c",
        vboot_dir + "firmware/lib/vboot_firmware.c",
        vboot_dir + "firmware/lib/cryptolib/rsa_utility.c",
#        vboot_dir + "firmware/lib/cryptolib/rsa.c",
        vboot_dir + "firmware/lib/utility.c"
        ]

extras = [
#        "--bounds-check",
         " -D NONDET_VARS",
         " -D CBMC",
        "--unwindset Memcpy.0:1050,modpowF4.0:1050"
        "--little-endian", "--64"
        ]

# ------------------------------------------------------------
# Build and Run the command
# ------------------------------------------------------------

commandString = "cbmc "

for s in includedDirs:
    commandString += "-I " + s + " "

commandString += testFile + " "

for s in includedFiles:
    commandString += s + " "

for s in extras:
    commandString += s + " "

print (commandString)

os.system(commandString)

