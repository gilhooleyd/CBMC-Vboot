#include "tty.h"
#include "tpm.h"
#include "tpm-info.h"
#include "tlcl.h"
#include "tpm_bootmode.h"
#include "gbb_header.h"
#include "tpm_error_messages.h"


extern locality;
// the PCR extend command
// first 2 bytes are tag
// next  4 bytes are size
// rest is data
uint8_t pcr_extend_cmd[] = {0x00, 0xC1, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
0x12, 0x13};

uint32_t badSend(const unsigned char *buf, uint32_t len) {
    int status, burstcnt = 0;
    uint32_t count = 0;
    terminal_writestring("Sending ");

    // request locality
    if (request_locality(locality) == -1)
        return -1;

    // tell the TPM that we will be writing a command now
    write8(STS_COMMAND_READY, STS(locality));

    // loop to write command
    while (count < len - 1) {
        // get the burst count
        burstcnt = readBurstCount();

        if (burstcnt == 0){
            delay(); /* wait for FIFO to drain */
        } else {
            for (; burstcnt > 0 && count < len - 1; burstcnt--) {
                write8(buf[count],
                        DATA_FIFO(locality));
                count++;
            }

            // wait while we are not in a valid state (overflow)
            while ( ( (status = read8(STS(locality)))
                        & STS_VALID) == 0) {}

            // break if the TPM is not expecting more data
            if ((status & STS_DATA_EXPECT) == 0) {
                terminal_writestring("Not expecting more data\n");
                return -1;
            }
        }
    }

    // Here is where we DONT write the last byte
//    write8(buf[count], DATA_FIFO(locality));

    // wait until valid state
    while ( ( (status = read8(STS(locality)))
                & STS_VALID) == 0) {}

    // Tell the TPM to execute the command
    // (we still have a byte waiting)
    write8(STS_GO, STS(locality));

    return len;
}

int kernel_main(void) {
    uint8_t in_digest[TPM_PCR_DIGEST];
    uint8_t ret_digest[TPM_PCR_DIGEST];
    uint8_t out_digest[TPM_PCR_DIGEST];
    int i;
    int res;

    terminal_init();
    terminal_writestring("TPM BAD CMD\n");
    for (i = 0; i < TPM_PCR_DIGEST; i++) {
        in_digest[i] = i;
    }

    TlclLibInit();
    terminal_writestring("Lib Init \n");
    TlclStartup();
    terminal_writestring("Lib Up \n");

    res = badSend(pcr_extend_cmd, 0x22);
    res = badSend(pcr_extend_cmd, 0x22);

//    terminal_writestring("Extend ");
//    printHex(res);
//    terminal_writestring("\n");
//    if (res != 0) {
//        terminal_writestring(tpm_error_table[res - 1].description);
//        terminal_writestring("\n");
//    }

    terminal_writestring("Done \n");
    while(1) {};
}
