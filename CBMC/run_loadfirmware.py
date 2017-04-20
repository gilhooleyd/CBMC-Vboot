import os
import argparse

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
        "--unwindset Memcpy.0:1050"
         " -D NONDET_VARS",
         " -D CBMC",
        ]

# ------------------------------------------------------------
# Build the Commandline arguments + test
# ------------------------------------------------------------
parser = argparse.ArgumentParser(description='Runs and checks loadFirmware. \n Test arguments are: \nbounds\nrollback\nrollback-rec\nhash\nrsa\ngoogle\n--malloc')

parser.add_argument('testname',  help='Required test name')
parser.add_argument('--malloc', action='store_true',
                            help='Enables memory allocator')
args = parser.parse_args()


if (args.testname == 'bounds'):
    extras.append("--bounds-check")
if (args.testname == 'rollback'):
    extras.append("-D ROLLBACK")
if (args.testname == 'rollback-rec'):
    extras.append("-D ROLLBACK_REC")
if (args.testname == 'hash'):
    extras.append("-D HASH")
if (args.testname == 'rsa'):
    extras.append("-D RSA")
if (args.testname == 'google'):
    includedFiles.append("test_common.c")
if (args.malloc):
    extras.append("-D MALLOC")

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

