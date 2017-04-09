#include <stdint.h>
#include "tpm.h"


// Public functions: fetch, decode, update, ...
void update(BIT_VEC cmd, BIT_VEC cmdaddr, BIT_VEC cmddata);
void printState();

uint32_t pcr_read_cmd[] = {0x00, 0xC1, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00}; 

int main(void) {
    int i = 0;

    update(WR, STS_ADDR, STS_COMMAND_READY);

    for (i = 0; i < sizeof(pcr_read_cmd); i++) {
        update(WR, FIFO_ADDR, pcr_read_cmd[i]);
    }
    printState();
}
