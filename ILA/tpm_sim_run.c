#include <stdint.h>

typedef  uint32_t BIT_VEC;

// Commands
int NOP = 0;
int RD  = 1;
BIT_VEC WR = 2;

// Fifo states
int FIFO_IDLE = 0;
int FIFO_ACCEPTING = 1;
int FIFO_FULL      = 2;
int FIFO_SENDING   = 3;
int FIFO_WORKING   = 4;

// Fifo Addresses
int ADDR = 0x0;

// TODO: Find max amount
int FIFO_MAX_AMT = 126;

// Flags that the status can output
int STS_VALID           = 0x80;
int STS_DATA_AVAIL      = 0x10;
int STS_DATA_EXPECT     = 0x08;

// Flags that can be written to status
int STS_GO              = 0x20;
BIT_VEC STS_COMMAND_READY   = 0x40;


// Public functions: fetch, decode, update, ...
void update(BIT_VEC cmd, BIT_VEC cmdaddr, BIT_VEC cmddata);
void printState();

uint32_t pcr_read_cmd[] = {0x00, 0xC1, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00}; 

int main(void) {
    BIT_VEC STS_ADDR = 0x24 + ADDR;
    int FIFO_ADDR = 0x18 + ADDR;
    int BURST_ADDR = 0x1 + ADDR;
    int i = 0;

    update(WR, STS_ADDR, STS_COMMAND_READY);
    printState();

    for (i = 0; i < sizeof(pcr_read_cmd); i++) {
        update(WR, pcr_read_cmd[i], FIFO_ACCEPTING);
    }
}
