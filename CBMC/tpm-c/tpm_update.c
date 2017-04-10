#include <stdio.h>
#include <stdint.h>

#include "tpm.h"

// State variables.
uint32_t dataout;
uint32_t fifo_in_amt;
uint32_t fifo_in_cmdsize;
uint32_t fifo_out_amt;
uint32_t fifo_state;
uint32_t fifo_sts;

uint8_t fifo_outdata[126];
uint8_t fifo_indata[126];
uint8_t pcr_data[20*24];

void tpm_runCmd();

void tpm_init() {
    dataout = 0;
    fifo_in_amt = 0;
    fifo_in_cmdsize = 0;
    fifo_out_amt = 0;
    fifo_state = 0;
    fifo_sts = 0;

    for (int i = 0; i < 126; i++) {
        fifo_outdata[i] = 0;
        fifo_indata[i] = 0;
    }
    for (int i = 0; i < 20*24; i++) {
        pcr_data[i] = 0;
    }
}

// Public functions: fetch, decode, update, ...
uint32_t tpm_update(uint32_t cmd, uint32_t cmdaddr, uint32_t cmddata) {
    uint32_t data_aval;
    uint32_t sts_valid;
    uint32_t data_expected;
    dataout = 0;

    // All Reg reads
    if (cmd == RD) {
        // Status Reg
        if (cmdaddr == STS_ADDR) {
            dataout = fifo_sts;
        }
        // Burst Reg
        if (cmdaddr == BURST_ADDR) {
            if (fifo_state == FIFO_ACCEPTING) {
                dataout = FIFO_MAX_AMT - fifo_in_amt;
            }
            if (fifo_state == FIFO_SENDING) {
                dataout = fifo_out_amt;
            }
        }
        // Fifo Reg
        else if (cmdaddr == FIFO_ADDR) {
            // Send Data
            if (fifo_state == FIFO_SENDING) {
                if (fifo_out_amt > 0) {
                    dataout = fifo_outdata[fifo_out_amt];
                    fifo_out_amt -= 1;
                }
            }
        }
    }
    // All Reg writes
    if (cmd == WR) {
        // Status Reg
        if (cmdaddr == STS_ADDR) {
            // Start Command
            if ((cmddata == STS_COMMAND_READY)) {
                for (int i = 0; i < FIFO_MAX_AMT; i++)
                    fifo_indata[i] = 0;
                fifo_state = FIFO_ACCEPTING;
                fifo_in_amt = 0;
                fifo_out_amt = 0;

            }
            // Run Command
            else if (cmddata == STS_GO) {
                tpm_runCmd();
            }
        }
        // Fifo Reg
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

    // Status Flags
    data_aval = (fifo_out_amt > 0) ? STS_DATA_AVAIL : 0;
    sts_valid = (fifo_state != FIFO_WORKING) ? STS_VALID : 0;
    data_expected = 0;
    if (((fifo_state == FIFO_ACCEPTING) && fifo_in_amt < 4) 
            || (fifo_in_amt < fifo_in_cmdsize)){
        data_expected = STS_DATA_EXPECT;
    }

    // Status byte 
    fifo_sts = data_aval | sts_valid | data_expected;
    return dataout;
}

void tpm_printState() {
    int i = 0;
    printf("FIFO_STATE   = %x\n", fifo_state);
    printf("FIFO_IN_AMT  = %x\n", fifo_in_amt);
    printf("FIFO_STS     = %x\n", fifo_sts);
    printf("FIFO_IN_CMDSIZE     = %x\n", fifo_in_cmdsize);

    for (i = 0; i < 126; i++)
        printf("IN %i \t = %x\n", i, fifo_indata[i]);
}

