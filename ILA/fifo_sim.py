import fifo_def

class fifo():

    def s_dict(self):
        return {
                'fifo_state' : self.fifo_state,
                'fifo_sts'   : self.fifo_sts,
                'fifo_in_amt'   : self.fifo_in_amt,
                'fifo_in_cmdsize' : self.fifo_in_cmdsize,
                'fifo_indata'  : self.fifo_indata,
                'fifo_out_amt'   : self.fifo_out_amt,
                'fifo_outdata'  : self.fifo_outdata,
                'dataout'  : self.dataout
                }

    def s_update(self, s_in):
        self.fifo_state = s_in['fifo_state']
        self.fifo_sts = s_in['fifo_sts']
        self.fifo_in_amt = s_in['fifo_in_amt']
        self.fifo_in_cmdsize = s_in['fifo_in_cmdsize']
        self.fifo_indata = s_in['fifo_indata']
        self.fifo_out_amt = s_in['fifo_out_amt']
        self.fifo_outdata = s_in['fifo_outdata']
        self.dataout = s_in['dataout']

    def __init__(self):
        self.all_state = [
                'fifo_state',
                'fifo_sts',
                'fifo_in_amt',
                'fifo_indata',
                'fifo_out_amt',
                'fifo_outdata',
                'dataout'
                ]
        self.fifo_state = 0
        self.fifo_sts = 0
        self.fifo_in_amt = 0
        self.fifo_in_cmdsize = 0
        self.fifo_indata = [0] * fifo_def.FIFO_MAX_AMT
        self.fifo_out_amt = 0
        self.fifo_outdata = [0] * fifo_def.FIFO_MAX_AMT
        self.dataout = 0
        return

    def simulate(self, s_in):
        cmd     = s_in['cmd']
        cmdaddr = s_in['cmdaddr']
        cmddata = s_in['cmddata']
        self.s_update(s_in)

        # defaults
        dataout = 0

        # Parse the I/O command from the CPU
        if cmd == fifo_def.RD:
            if cmdaddr == fifo_def.STS_ADDR:
                dataout = self.fifo_sts
            if cmdaddr == BURST_ADDR:
                if self.fifo_state == FIFO_ACCEPTING:
                    dataout = FIFO_MAX_AMT - fifo_in_amt
                if self.fifo_state == FIFO_SENDING:
                    dataout = self.fifo_out_amt
            # If we are reading the fifo data, then set dataout and decrease amt
            elif cmdaddr == fifo_def.FIFO_ADDR:
                if self.fifo_out_amt > 0:
                    dataout = self.fifo_outdata[self.fifo_out_amt]
                    self.fifo_out_amt -= 1
        if cmd == fifo_def.WR:
            if cmdaddr == fifo_def.STS_ADDR:
                if (cmddata == fifo_def.STS_COMMAND_READY):
                    self.fifo_state = fifo_def.FIFO_ACCEPTING
                    self.fifo_in_amt = 0
                    self.fifo_out_amt = 0
                # TODO: Here's where the magic will happen
                elif cmddata == fifo_def.STS_GO:
                    self.fifo_state = fifo_def.FIFO_IDLE
                    self.fifo_in_amt = 0
                    self.fifo_out_amt = 0
            elif cmdaddr == fifo_def.FIFO_ADDR:
                # If the fifo isn't full update it
                if self.fifo_state != fifo_def.FIFO_FULL:
                    print "Updating State"
                    # If this is 5th byte its the cmd size byte
                    if (self.fifo_in_amt == 4):
                        self.fifo_in_cmdsize = cmddata

                    self.fifo_indata[self.fifo_in_amt] = cmddata
                    self.fifo_in_amt += 1
                    # If fifo is full now reflect that
                    if self.fifo_in_amt == fifo_def.FIFO_MAX_AMT:
                        self.fifo_state = fifo_def.FIFO_FULL
                    # TODO: Look and see if we need to do something with command length

        s_in['dataout'] = dataout
        self.dataout = dataout
        # Generate the status flags
        data_aval = fifo_def.STS_DATA_AVAIL if (self.fifo_out_amt > 0) else 0
        sts_valid = fifo_def.STS_VALID if (self.fifo_state != fifo_def.FIFO_WORKING) else 0

        data_expected = 0
        if ((self.fifo_state == fifo_def.FIFO_ACCEPTING) and self.fifo_in_amt < 4) \
                or (self.fifo_in_amt < self.fifo_in_cmdsize):
            data_expected = fifo_def.STS_DATA_EXPECT

        self.fifo_sts = data_aval | sts_valid | data_expected

        return self.s_dict()

if __name__ == '__main__':
    f = fifo()
    s_in = f.s_dict()
    s_in['cmd'] = fifo_def.WR
    s_in['cmdaddr'] = fifo_def.FIFO_ADDR
    s_in['cmddata'] = 1
    s_in['fifo_state'] = fifo_def.FIFO_ACCEPTING
    print s_in
    if (s_in['fifo_sts'] & fifo_def.STS_DATA_AVAIL):
        print "data available"
    if (s_in['fifo_sts'] & fifo_def.STS_VALID):
        print "data valid"
    if (s_in['fifo_sts'] & fifo_def.STS_DATA_EXPECT):
        print "data expected"
