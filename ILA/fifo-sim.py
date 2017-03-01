NOP = 0
RD  = 1
WR  = 2
FIFO_IDLE = 0
FIFO_ACCEPTING = 1
FIFO_FULL      = 2
FIFO_SENDING   = 3
FIFO_WORKING   = 4

ADDR = 0xFED40000
STS_ADDR = 0x24 + ADDR
FIFO_ADDR = 0x18 + ADDR
BURST_ADDR = 0x1 + ADDR

# TODO: Find max amount
FIFO_MAX_AMT = 126

class fifo():

    def s_dict(self):
        return {
                'fifo_state' : self.fifo_state,
                'fifo_sts'   : self.fifo_sts,
                'fifo_in_amt'   : self.fifo_in_amt,
                'fifo_in_cmdsize' : self.fifo_in_cmdsize,
                'fifo_indata'  : self.fifo_indata,
                'fifo_out_amt'   : self.fifo_out_amt,
                'fifo_outdata'  : self.fifo_outdata
                }

    def s_update(self, s_in):
        self.fifo_state = s_in['fifo_state']
        self.fifo_sts = s_in['fifo_sts']
        self.fifo_in_amt = s_in['fifo_in_amt']
        self.fifo_in_cmdsize = s_in['fifo_in_cmdsize']
        self.fifo_indata = s_in['fifo_indata']
        self.fifo_out_amt = s_in['fifo_out_amt']
        self.fifo_outdata = s_in['fifo_outdata']

    def __init__(self):
        self.fifo_state = 0
        self.fifo_sts = 0
        self.fifo_in_amt = 0
        self.fifo_in_cmdsize = 0
        self.fifo_indata = [0] * FIFO_MAX_AMT
        self.fifo_out_amt = 0
        self.fifo_outdata = [0] * FIFO_MAX_AMT
        return

    def simulate(self, s_in):
        cmd     = s_in['cmd']
        cmdaddr = s_in['cmdaddr']
        cmddata = s_in['cmddata']
        self.s_update(s_in)

        # defaults
        dataout = 0

        # Parse the I/O command from the CPU
        if cmd == RD:
            if cmdaddr == STS_ADDR:
                dataout = self.fifo_sts
            # If we are reading the fifo data, then set dataout and decrease amt
            elif cmdaddr == FIFO_ADDR:
                if self.out_amt > 0:
                    dataout = self.fifo_outdata[self.fifo_out_amt]
                    self.fifo_out_amt -= 1
        if cmd == WR:
            if cmdaddr == STS_ADDR:
                if (cmddata == STS_CMD_READY):
                    self.fifo_state = FIFO_ACCEPTING
                    self.fifo_in_amt = 0
                    self.fifo_out_amt = 0
                # TODO: Here's where the magic will happen
                elif cmddata == STS_GO:
                    self.fifo_state = FIFO_IDLE
                    self.fifo_in_amt = 0
                    self.fifo_out_amt = 0
            elif cmdaddr == FIFO_ADDR:
                # If the fifo isn't full update it
                if self.fifo_state != FIFO_FULL:
                    # If this is 5th byte its the cmd size byte
                    if (self.fifo_in_amt == 4):
                        self.fifo_in_cmdsize = cmddata

                    self.fifo_indata[self.fifo_in_amt] = cmddata
                    self.fifo_in_amt += 1
                    # If fifo is full now reflect that
                    if self.fifo_in_amt == FIFO_MAX_AMT:
                        self.fifo_state = FIFO_FULL
                    # TODO: Look and see if we need to do something with command length

        # Generate the status flags
        data_aval = 0x10 if (self.fifo_out_amt > 0) else 0
        sts_valid = 0x80 if (self.fifo_state != FIFO_WORKING) else 0

        data_expected = 0
        if ((self.fifo_state == FIFO_ACCEPTING) and self.fifo_in_amt < 4) \
                or (self.fifo_in_amt < self.fifo_in_cmdsize):
            data_expected = 0x08

        self.fifo_sts = data_aval | sts_valid | data_expected

        return self.s_dict()

if __name__ == '__main__':
    f = fifo()
    s_in = f.s_dict()
    s_in['cmd'] = WR
    s_in['cmdaddr'] = FIFO_ADDR
    s_in['cmddata'] = 1
    s_in['fifo_state'] = FIFO_ACCEPTING
    s_in = f.simulate(s_in)
#    s_in["fifo_in_amt"] = 4
#    s_in['cmd'] = WR
#    s_in['cmdaddr'] = FIFO_ADDR
#    s_in['cmddata'] = 123
#    s_in = f.simulate(s_in)
    print s_in
    if (s_in['fifo_sts'] & 0x10):
        print "data available"
    if (s_in['fifo_sts'] & 0x80):
        print "data valid"
    if (s_in['fifo_sts'] & 0x08):
        print "data expected"
