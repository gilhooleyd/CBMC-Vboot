# Commands
NOP = 0
RD  = 1
WR  = 2

# Fifo states
FIFO_IDLE = 0
FIFO_ACCEPTING = 1
FIFO_FULL      = 2
FIFO_SENDING   = 3
FIFO_WORKING   = 4

# Fifo Addresses
ADDR = 0x0
STS_ADDR = 0x24 + ADDR
FIFO_ADDR = 0x18 + ADDR
BURST_ADDR = 0x1 + ADDR

# TODO: Find max amount
FIFO_MAX_AMT = 126

# Flags that the status can output
STS_VALID           = 0x80
STS_DATA_AVAIL      = 0x10
STS_DATA_EXPECT     = 0x08

# Flags that can be written to status
STS_GO              = 0x20
STS_COMMAND_READY   = 0x40
