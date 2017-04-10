#include <stdint.h>
#include <stdio.h>
#include <openssl/sha.h>

#include "tpm.h"

typedef  uint32_t BIT_VEC;

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

void pcr_extend() {
    int CMD_SIZE = 0x1e;
    int pcr_index = fifo_indata[13];
    uint8_t extend_data[40];
    uint8_t *chosen_pcr = &pcr_data[20 * pcr_index];
    int j;

    // Fill first 20 bytes with old PCR data
    for (int i = 0; i < 20; i++) {
        extend_data[i] = chosen_pcr[i];
    }
    // Fill next 20 bytes with command data    
    j = 14;
    for (int i = 20; i < 40; i++) {
        extend_data[i] = fifo_indata[j];
        j += 1;
    }
    // Take SHA hash
    SHA1(extend_data, 40, chosen_pcr);

    // Set the output data
    // set the output tag
    fifo_outdata[CMD_SIZE - 1] = 0xc4;
    // set the output size
    fifo_outdata[CMD_SIZE - 5] = CMD_SIZE;
    fifo_out_amt    = CMD_SIZE;
    // set the output data
    for (int i = 0; i < 20; i ++)
        fifo_outdata[CMD_SIZE - 10 - i] = chosen_pcr[i];
}

void pcr_read() {
    int CMD_SIZE = 0x1e;
    int pcr_index = fifo_indata[13];
    // set the output tag
    fifo_outdata[CMD_SIZE - 1] = 0xc4;
    // set the output size
    fifo_outdata[CMD_SIZE - 5] = CMD_SIZE;
    fifo_out_amt    = CMD_SIZE;
    // set the output data
    for (int i = 0; i < 20; i++) {
        fifo_outdata[CMD_SIZE - 10 - i] = pcr_data[20 * pcr_index + i];
    }
}

void runCmd() {
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
                    pcr_read(fifo_indata, fifo_outdata);
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

// Public functions: fetch, decode, update, ...
uint32_t update(uint32_t cmd, uint32_t cmdaddr, uint32_t cmddata) {
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
                runCmd();
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

void printState() {
    int i = 0;
    printf("FIFO_STATE   = %x\n", fifo_state);
    printf("FIFO_IN_AMT  = %x\n", fifo_in_amt);
    printf("FIFO_STS     = %x\n", fifo_sts);
    printf("FIFO_IN_CMDSIZE     = %x\n", fifo_in_cmdsize);

    for (i = 0; i < 126; i++)
        printf("IN %i \t = %x\n", i, fifo_indata[i]);
}

