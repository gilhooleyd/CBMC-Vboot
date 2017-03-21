import os

includedDirs = [
        "../../host/lib/include",
        "../../firmware/include/",
        "../../firmware/lib/cryptolib/include/",
        "../../firmware/lib/include/",
        "../"
        ]

testFile = "test_loadfirmware.c"

includedFiles = [
        # include file below to do Google's unit test assertions
#        "test_common.c",

        "../../firmware/stub/utility_stub.c",
        "../../firmware/lib/vboot_common.c",
        "../../firmware/lib/vboot_nvstorage.c",
        "../../firmware/lib/vboot_common_init.c",
        "../../firmware/lib/crc8.c",
#        "../../firmware/lib/region-fw.c",
        "../../firmware/lib/vboot_firmware.c"
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

