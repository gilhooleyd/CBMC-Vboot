#include <stdint.h>
#include <stdio.h>

#include "tpm.h"

typedef  uint32_t BIT_VEC;

// State variables.
uint32_t dataout;
uint32_t fifo_in_amt;
uint32_t fifo_in_cmdsize;
uint32_t fifo_out_amt;
uint32_t fifo_state;
uint32_t fifo_sts;

uint32_t fifo_outdata[126];
uint32_t fifo_indata[126];

// Public functions: fetch, decode, update, ...
void update(uint32_t cmd, uint32_t cmdaddr, uint32_t cmddata) {
    uint32_t data_aval;
    uint32_t sts_valid;
    uint32_t data_expected;
    dataout = 0;

    if (cmd == RD) {
        if (cmdaddr == STS_ADDR) {
            dataout = fifo_sts;
        }
        if (cmdaddr == BURST_ADDR) {
            if (fifo_state == FIFO_ACCEPTING) {
                dataout = FIFO_MAX_AMT - fifo_in_amt;
            }
            if (fifo_state == FIFO_SENDING) {
                dataout = fifo_out_amt;
            }
        }
        else if (cmdaddr == FIFO_ADDR) {
            if (fifo_state == FIFO_SENDING) {
                if (fifo_out_amt > 0) {
                    dataout = fifo_outdata[fifo_out_amt];
                    fifo_out_amt -= 1;
                }
            }
        }
    }
    if (cmd == WR) {
        if (cmdaddr == STS_ADDR) {
            if ((cmddata == STS_COMMAND_READY)) {
                for (int i = 0; i < FIFO_MAX_AMT; i++)
                    fifo_indata[i] = 0;
                fifo_state = FIFO_ACCEPTING;
                fifo_in_amt = 0;
                fifo_out_amt = 0;

            }
            else if (cmddata == STS_GO) {
//                runCmd();
            }
        }
        else if (cmdaddr == FIFO_ADDR) {
            if (fifo_state != FIFO_FULL) {
                if ((fifo_in_amt == 5)) {
                    fifo_in_cmdsize = cmddata;
                }

                fifo_indata[fifo_in_amt] = cmddata;
                fifo_in_amt += 1;
                if (fifo_in_amt == FIFO_MAX_AMT) {
                    fifo_state = FIFO_FULL;
                }
            }
        }
    }
    data_aval = (fifo_out_amt > 0) ? STS_DATA_AVAIL : 0;
    sts_valid = (fifo_state != FIFO_WORKING) ? STS_VALID : 0;
    data_expected = 0;

    if (((fifo_state == FIFO_ACCEPTING) && fifo_in_amt < 4) 
            || (fifo_in_amt < fifo_in_cmdsize)){
        data_expected = STS_DATA_EXPECT;
    }

    fifo_sts = data_aval | sts_valid | data_expected;
}

void printState() {
    int i = 0;
    printf("FIFO_STATE   = %zu\n", fifo_state);
    printf("FIFO_IN_AMT  = %d\n", fifo_in_amt);
    printf("FIFO_STS     = %zu\n", fifo_sts);

    for (i = 0; i < 126; i++)
        printf("IN %i \t = %d\n", i, fifo_indata[i]);
}
