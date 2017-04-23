#include <stdint.h>
#include <stdio.h>

#include "ila-tpm.h"

int pcr_extend();
int pcr_read();

void ilarunCmd(struct model_fifo *tpm) {
        int cmd_worked = 0;
        printf("Run Command\n");
        printf("%d %d %d\n", tpm->fifo_state, tpm->fifo_in_amt, tpm->fifo_in_cmdsize);
        // Only run commands if we are accepting data
        //  and the in amount is the commandsize
        if (tpm->fifo_state == FIFO_ACCEPTING) {
                printf("Fifo State\n");
            if (tpm->fifo_in_amt == tpm->fifo_in_cmdsize) {
                printf("Right Size\n");
                // PCR_extend command
                if (tpm->fifo_indata[6] == 0 &&
                        tpm->fifo_indata[7] == 0 && 
                        tpm->fifo_indata[8] == 0 && 
                        tpm->fifo_indata[9] == 0x14 ) {
                    int ret = pcr_extend(tpm);
                    if (ret == 0)
                        cmd_worked = 1;
                    printf("PCR EXTEND\n");
                }
                if (tpm->fifo_indata[6] == 0 && 
                        tpm->fifo_indata[7] == 0 && 
                        tpm->fifo_indata[8] == 0 && 
                        tpm->fifo_indata[9] == 0x15 ) {
                    int ret = pcr_read(tpm);
                    if (ret == 0)
                        cmd_worked = 1;
                    printf("PCR EXTEND\n");
                }
            }
        }

        // Send malformed command if broken
        if (cmd_worked == 0) {
            int CMD_SIZE = 9;
            tpm->fifo_state = FIFO_SENDING;
            tpm->fifo_in_amt = 0;
            // Set the output data
            // set the output tag
            tpm->fifo_outdata[CMD_SIZE - 1] = 0xc4;
            // set the output size
            tpm->fifo_outdata[CMD_SIZE - 5] = CMD_SIZE;
            tpm->fifo_outdata[CMD_SIZE - CMD_SIZE + 1] = 0xFF;
            tpm->fifo_out_amt    = CMD_SIZE;
        }
        // Otherwise change state to sending
        else {
            tpm->fifo_state = FIFO_SENDING;
        }
        printf("END RUN CMD\n");
}
