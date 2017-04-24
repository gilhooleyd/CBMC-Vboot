#include "rollback_index.h"

#ifdef ERROR_HANDLING
#define ROLLBACKFIRMWAREWRITE 1
#define CBMC_TPM_ERROR_TEST 1
#endif

int main(void) {
    int dev_mode = nondet_int();
    int disable_dev_request = nondet_int();
    int clear_tpm_owner_request = nondet_int();

    int rollback_version = nondet_int();

    uint32_t ret = 1;

    uint32_t developer_mode = nondet_int();
    uint32_t recovery_mode = nondet_int();
    uint64_t keyblock_flags = nondet_int();

    RollbackSpaceFirmware rsf;
    rsf.struct_version = nondet_int();
    rsf.flags = nondet_int();
    rsf.fw_versions = nondet_int();

#ifdef SETUP_TPM
    SetupTPM(dev_mode, disable_dev_request, clear_tpm_owner_request, 
            &rsf);
#endif
#ifdef ROLLBACKFIRMWAREWRITE
    ret = RollbackFirmwareWrite(rollback_version);
#endif
    ret = RollbackFirmwareLock();
#ifdef SETTPMBOOTMODESTATE
    ret = SetTPMBootModeState(developer_mode, recovery_mode,
            keyblock_flags, NULL);
#endif
}
