#include <stdint.h>
#include <stdio.h>
#include <openssl/sha.h>

#include "ila-tpm.h"

uint8_t pcr_data[20*24];

int pcr_extend(struct model_fifo *tpm) {
    int CMD_SIZE = 0x1e;
    int pcr_index = tpm->fifo_indata[13];
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
        extend_data[i] = tpm->fifo_indata[j];
        j += 1;
    }
    printf("START SHA\n");
    // Take SHA hash
    SHA1(extend_data, 40, chosen_pcr);
    printf("DONE SHA\n");

    // Set the output data
    // set the output tag
    tpm->fifo_outdata[CMD_SIZE - 1] = 0xc4;
    // set the output size
    tpm->fifo_outdata[CMD_SIZE - 5] = CMD_SIZE;
    tpm->fifo_out_amt    = CMD_SIZE;
    // set the output data
    for (int i = 0; i < 20; i ++)
        tpm->fifo_outdata[CMD_SIZE - 10 - i] = chosen_pcr[i];
    printTpm();
    return 0;
}

int pcr_read(struct model_fifo *tpm) {
    int CMD_SIZE = 0x1e;
    int pcr_index = tpm->fifo_indata[13];

    if (pcr_index > 23)
        return -1;

    // set the output tag
    tpm->fifo_outdata[CMD_SIZE - 1] = 0xc4;
    // set the output size
    tpm->fifo_outdata[CMD_SIZE - 5] = CMD_SIZE;
    tpm->fifo_out_amt    = CMD_SIZE;
    // set the output data
    for (int i = 0; i < 20; i++) {
        tpm->fifo_outdata[CMD_SIZE - 10 - i] = pcr_data[20 * pcr_index + i];
    }
    return 0;
}
