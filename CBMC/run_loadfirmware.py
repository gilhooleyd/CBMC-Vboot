import os

vboot_dir = "../_vboot_reference/"

includedDirs = [
        vboot_dir + "host/lib/include",
        vboot_dir + "firmware/include/",
        vboot_dir + "firmware/lib/cryptolib/include/",
        vboot_dir + "firmware/lib/include/",
        vboot_dir + "tests"
        ]

testFile = "test_loadfirmware.c"

includedFiles = [
        # include file below to do Google's unit test assertions
#        "test_common.c",

        vboot_dir + "firmware/stub/utility_stub.c",
        vboot_dir + "firmware/lib/vboot_common.c",
        vboot_dir + "firmware/lib/vboot_nvstorage.c",
        vboot_dir + "firmware/lib/vboot_common_init.c",
        vboot_dir + "firmware/lib/crc8.c",
#        vboot_dir + "firmware/lib/region-fw.c",
        vboot_dir + "firmware/lib/vboot_firmware.c"
        ]

extras = [
#        "--bounds-check",
         " -D NONDET_VARS",
         " -D CBMC",
        "--unwindset Memcpy.0:1050"
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

