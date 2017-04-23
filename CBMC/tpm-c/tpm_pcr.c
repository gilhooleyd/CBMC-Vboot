#include <stdint.h>
#include <stdio.h>
#include <openssl/sha.h>

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

int pcr_extend() {
    int CMD_SIZE = 0x1e;
    int pcr_index = fifo_indata[13];
    uint8_t extend_data[40];
    uint8_t *chosen_pcr = &pcr_data[20 * pcr_index];
    int j;

    if (pcr_index > 23)
        return -1;

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
    return 0;
}

int pcr_read() {
    int CMD_SIZE = 0x1e;
    int pcr_index = fifo_indata[13];

    if (pcr_index > 23)
        return -1;

    // set the output tag
    fifo_outdata[CMD_SIZE - 1] = 0xc4;
    // set the output size
    fifo_outdata[CMD_SIZE - 5] = CMD_SIZE;
    fifo_out_amt    = CMD_SIZE;
    // set the output data
    for (int i = 0; i < 20; i++) {
        fifo_outdata[CMD_SIZE - 10 - i] = pcr_data[20 * pcr_index + i];
    }
    return 0;
}
