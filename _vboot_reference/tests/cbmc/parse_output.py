import re

gbb_line = ""
block_hvm = re.compile(".*vblock\[.*header_version_major=");
preamble_hvm = re.compile(".*mpreamble\[.*header_version_major=");

with open("output") as f:
    for line in f:
        if ("gbb_flags" in line) and (gbb_line == "" ):
            print line
            gbb_line = line
        elif (block_hvm.match(line)):
            print line
        elif (preamble_hvm.match(line)):
            print line

i = gbb_line.find("=")
k = gbb_line[i:].find(" ")
gbb_flags = int(gbb_line[i + 1: i + k])

print("GBB FLAGS : " + str(gbb_flags))

gbb_def = {
        "DEV_SCREEN_SHORT_DELAY" : 0x00000001,
        "LOAD_OPTION_ROMS" : 0x00000002,
        "ENABLE_ALTERNATE_OS" : 0x00000004,
        "FORCE_DEV_SWITCH_ON" : 0x00000008,
        "FORCE_DEV_BOOT_USB" : 0x00000010,
        "DISABLE_FW_ROLLBACK_CHECK" : 0x00000020,
        "ENTER_TRIGGERS_TONORM" : 0x00000040,
        "FORCE_DEV_BOOT_LEGACY" : 0x00000080,
        "FAFT_KEY_OVERIDE" : 0x00000100,
        "DISABLE_EC_SOFTWARE_SYNC" : 0x00000200,
        "DEFAULT_DEV_BOOT_LEGACY" : 0x00000400,
        "DISABLE_PD_SOFTWARE_SYNC" : 0x00000800,
        "DISABLE_LID_SHUTDOWN" : 0x00001000,
        "FORCE_DEV_BOOT_FASTBOOT_FULL_CAP" : 0x00002000,
        "ENABLE_SERIAL" : 0x00004000
        }

for key in gbb_def:
    isValid = gbb_flags & gbb_def[key]
    if isValid != 0:
        isValid = 1
    print (key + " " * (50 - len(key)) + str(isValid))

