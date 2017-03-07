#include "tlcl.h"
#include "tpm_bootmode.h"
#include "gbb_header.h"
#include <stdio.h>
#include <assert.h>

int main(void) {
    uint8_t in_digest[TPM_PCR_DIGEST];
    uint8_t ret_digest[TPM_PCR_DIGEST];
    uint8_t out_digest[TPM_PCR_DIGEST];
    int i;

    for (i = 0; i < TPM_PCR_DIGEST; i++) {
        in_digest[i] = i;
    }

    TlclLibInit();
    TlclStartup();
    TlclExtend (0, in_digest,  out_digest);
    TlclPCRRead(0, ret_digest, TPM_PCR_DIGEST);

    for (i = 0; i < TPM_PCR_DIGEST; i++) {
        assert(ret_digest[i] == out_digest[i]);
        printf("PCR BYTES %d \t 0x%x\n", i, out_digest[i]);
    }

    printf("Result: %d\n", SetTPMBootModeState(1, 1, 0, NULL));
}
