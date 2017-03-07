
#include "tty.h"
#include "tpm.h"
#include "tlcl.h"
#include "tpm_bootmode.h"
#include "gbb_header.h"


int kernel_main(void) {
    uint8_t in_digest[TPM_PCR_DIGEST];
    uint8_t ret_digest[TPM_PCR_DIGEST];
    uint8_t out_digest[TPM_PCR_DIGEST];
    int i;

    terminal_init();
    terminal_writestring("TPM PCR\n");
    for (i = 0; i < TPM_PCR_DIGEST; i++) {
        in_digest[i] = i;
    }

    TlclLibInit();
    terminal_writestring("Lib Init \n");
    TlclStartup();
    terminal_writestring("Lib Up \n");
    TlclExtend (0, in_digest,  out_digest);
    terminal_writestring("Extend \n");
    TlclPCRRead(0, ret_digest, TPM_PCR_DIGEST);
    terminal_writestring("Read \n");

    for (i = 0; i < TPM_PCR_DIGEST; i++) {
        if(!(ret_digest[i] == out_digest[i])) {
            printHex(i);
            terminal_writestring(" ");
            printHex(ret_digest[i]);
            terminal_writestring(" ");
            printHex(out_digest[i]);
            terminal_writestring("\n");
        }
    }

    while(1) {};
}
