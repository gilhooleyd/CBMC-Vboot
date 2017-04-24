import os

vboot_dir = "../_vboot_reference/"

includedDirs = [
        "./tpm-c/",
        vboot_dir + "firmware/lib/tpm_lite/include/",

        vboot_dir + "host/lib/include",
        vboot_dir + "firmware/include/",
        vboot_dir + "firmware/lib/cryptolib/include/",
        vboot_dir + "firmware/lib/include/",
        vboot_dir + "tests"
        ]

testFile = "test_selectFw.c"

includedFiles = [
        # include file below to do Google's unit test assertions
#        "test_common.c",
        "./tpm-fw.c",
        "./tpm-c/tpm_update.c",
        "./tpm-c/tpm_run_cmd.c",
        "./tpm-c/tpm_pcr.c",
        vboot_dir + "firmware/lib/tpm_bootmode.c",
        vboot_dir + "firmware/lib/tpm_lite/tlcl.c",

        vboot_dir + "firmware/stub/utility_stub.c",
        vboot_dir + "firmware/lib/vboot_api_firmware.c",
        vboot_dir + "firmware/lib/vboot_common.c",
        vboot_dir + "firmware/lib/vboot_nvstorage.c",
        vboot_dir + "firmware/lib/vboot_common_init.c",
        vboot_dir + "firmware/lib/crc8.c",
#        vboot_dir + "firmware/lib/region-fw.c",
        vboot_dir + "firmware/lib/vboot_firmware.c"
        ]

extras = [
#        "--bounds-check",
  "--unwindset send.0:40,send.1:3,send.2:3,send.3:3,recv_helper.0:40,recv_helper.1:2",
         " -D NONDET_VARS",
         " -D CBMC",
        "--unwindset Memcpy.0:1050"
        ]

# ------------------------------------------------------------
# Build the Commandline arguments + test
# ------------------------------------------------------------

parser.add_argument('testname',  help='Required test name')
parser.add_argument('--malloc', action='store_true',
                            help='Enables memory allocator')
args = parser.parse_args()


if (args.testname == 'liveness'):
    extras.append("--bounds-check")
if (args.testname == 'tpm_lock'):
    extras.append("-D TPM_LOCK")
if (args.testname == 'loadfirmware'):
    extras.append("-D LOAD_FIRMWARE")
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

