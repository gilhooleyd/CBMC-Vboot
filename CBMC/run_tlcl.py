import os
import argparse

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

testFile = "test_tlcl.c"

includedFiles = [
        # include file below to do Google's unit test assertions
#        "test_common.c",
        "./tpm-c/tpm_update.c",
        "./tpm-c/tpm_run_cmd.c",
        "./tpm-c/tpm_pcr.c",
        "./tpm-fw.c",
        vboot_dir + "firmware/lib/tpm_lite/tlcl.c",
        vboot_dir + "firmware/lib/rollback_index.c ",
        vboot_dir + "firmware/lib/tpm_bootmode.c",

        vboot_dir + "firmware/stub/utility_stub.c",
        vboot_dir + "firmware/lib/vboot_common.c",
        vboot_dir + "firmware/lib/vboot_nvstorage.c",
        vboot_dir + "firmware/lib/vboot_common_init.c",
        vboot_dir + "firmware/lib/crc8.c",
#        vboot_dir + "firmware/lib/region-fw.c",
        vboot_dir + "firmware/lib/vboot_firmware.c"
        ]

extras = [
#        "--show-loops"
  "--unwindset send.0:40,send.1:3,send.2:3,send.3:3,recv_helper.0:40,recv_helper.1:2",
         " -D CBMC_TPM_ERROR_TEST",
         " -D CHROMEOS_ENVIRONMENT",
         " -D NONDET_VARS",
         " -D CBMC",
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

