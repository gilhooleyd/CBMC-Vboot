#include <stdio.h>
#include <stdint.h>
#include "ila-tpm.h"

extern struct model_fifo tpm; 
void ilarunCmd(struct model_fifo *tpm);
void update(struct model_fifo* this, BIT_VEC cmd, BIT_VEC cmdaddr, BIT_VEC cmddata);

// Public functions: fetch, decode, update, ...
uint32_t tpm_update(BIT_VEC cmd, BIT_VEC cmdaddr, BIT_VEC cmddata) {
    int needsILA = 1;
    if (cmd == WR) {
        if (cmdaddr == STS_ADDR) {
            if (cmddata == STS_COMMAND_READY) {
                tpm.fifo_state = FIFO_ACCEPTING;
                tpm.fifo_in_amt = 0;
                tpm.fifo_out_amt = 0;
                needsILA = 0;
            }
            else if (cmddata == STS_GO) {
                printTpm();
                ilarunCmd(&tpm);
                needsILA = 0;
            }
        }
        else if (cmdaddr == FIFO_ADDR) {
            if (tpm.fifo_in_amt == 5)
                tpm.fifo_in_cmdsize = cmddata;
        }
    }
    if (cmd == RD) {
        if (cmdaddr == FIFO_ADDR) {
            if (tpm.fifo_state = FIFO_SENDING) {
                if (tpm.fifo_out_amt > 0) {
                    uint32_t dataout = 
                        tpm.fifo_outdata[tpm.fifo_out_amt];
                    tpm.fifo_out_amt -= 1;
                    return dataout;
                }
                else
                    return 0xDE;
            }
        }
        if (cmdaddr == BURST_ADDR) {
            if (tpm.fifo_state = FIFO_SENDING) {
                return tpm.fifo_out_amt;
            }
            else
                return 256;
        }
    }
    if (needsILA) {
        update(&tpm, cmd, cmdaddr, cmddata);
    }
}
