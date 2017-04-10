#include <stdint.h>
#include <stdio.h>

#include "tpm.h"

extern uint32_t dataout;
extern uint32_t fifo_in_amt;
extern uint32_t fifo_in_cmdsize;
extern uint32_t fifo_out_amt;
extern uint32_t fifo_state;
extern uint32_t fifo_sts;

extern uint8_t fifo_outdata[126];
extern uint8_t fifo_indata[126];
extern uint8_t pcr_data[20*24];

void pcr_extend();
void pcr_read();

void tpm_runCmd() {
        int cmd_worked = 0;
        printf("Run Command\n");
        printf("%d %d %d\n", fifo_state, fifo_in_amt, fifo_in_cmdsize);
        // Only run commands if we are accepting data
        //  and the in amount is the commandsize
        if (fifo_state == FIFO_ACCEPTING) {
                printf("Fifo State\n");
            if (fifo_in_amt == fifo_in_cmdsize) {
                printf("Right Size\n");
                // PCR_extend command
                if (fifo_indata[6] == 0 &&
                        fifo_indata[7] == 0 && 
                        fifo_indata[8] == 0 && 
                        fifo_indata[9] == 0x14 ) {
                    cmd_worked = 1;
                    pcr_extend(fifo_indata, fifo_outdata);
                    printf("PCR EXTEND\n");
                }
                if (fifo_indata[6] == 0 && 
                        fifo_indata[7] == 0 && 
                        fifo_indata[8] == 0 && 
                        fifo_indata[9] == 0x15 ) {
                    cmd_worked = 1;
                    pcr_read();
                }
            }
        }

        // Reset state if we had a malformed command
        if (cmd_worked == 0) {
            fifo_state = FIFO_IDLE;
            fifo_in_amt = 0;
            fifo_out_amt = 0;
        }
        // Otherwise change state to sending
        else {
            fifo_state = FIFO_SENDING;
        }
}
