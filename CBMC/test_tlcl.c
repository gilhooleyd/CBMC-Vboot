
/* Flags for firmware space */
/*
 * Last boot was developer mode.  TPM ownership is cleared when transitioning
 * to/from developer mode.
 */
#define FLAG_LAST_BOOT_DEVELOPER 0x01
/*
 * Some systems may not have a dedicated dev-mode switch, but enter and leave
 * dev-mode through some recovery-mode magic keypresses. For those systems, the
 * dev-mode "switch" state is in this bit (0=normal, 1=dev). To make it work, a
 * new flag is passed to VbInit(), indicating that the system lacks a physical
 * dev-mode switch. If a physical switch is present, this bit is ignored.
 */
#define FLAG_VIRTUAL_DEV_MODE_ON 0x02

int main(void) {
    int dev_mode = nondet_int();
    int disable_dev_request = nondet_int();
    int clear_tpm_owner_request = nondet_int();
    RollbackSpaceFirmware rsf;
    rsf.struct_version = nondet_int();
    rsf.flags = nondet_int();
    rsf.fw_versions = nondet_int();

    SetupTPM(dev_mode, disable_dev_request, clear_tpm_owner_request, 
            &rsf);
}
