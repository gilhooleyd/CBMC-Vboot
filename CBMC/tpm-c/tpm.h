#ifndef EXAMPLE_H_
#define EXAMPLE_H_
typedef  uint32_t BIT_VEC;

// Commands
#define NOP 0
#define RD 1
#define WR 2

// Fifo states
#define FIFO_IDLE 0
#define FIFO_ACCEPTING 1
#define FIFO_FULL      2
#define FIFO_SENDING   3
#define FIFO_WORKING   4

// Fifo Addresses
#define ADDR 0

#define FIFO_MAX_AMT 126

// Flags that the status can output
#define STS_VALID 0x80
#define STS_DATA_AVAIL 0x10
#define STS_DATA_EXPECT 0x08

// Flags that can be written to status
#define STS_GO 0x20
#define STS_COMMAND_READY 0x40

#define STS_ADDR 0x24 + ADDR
#define FIFO_ADDR 0x18 + ADDR
#define BURST_ADDR 0x1 + ADDR

void tpm_init();

uint32_t tpm_update(BIT_VEC cmd, BIT_VEC cmdaddr, BIT_VEC cmddata);

void tpm_printState();

#endif
