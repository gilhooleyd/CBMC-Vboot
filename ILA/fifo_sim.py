import fifo_def
from tpm_cmd_pcr import pcr_extend, pcr_read

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

    def runCmd(self):
        cmd_worked = 0
        # Only run commands if we are accepting data
        #  and the in amount is the commandsize
        if self.fifo_state == fifo_def.FIFO_ACCEPTING:
            if self.fifo_in_amt == self.fifo_in_cmdsize:
                # PCR_extend command
                if self.fifo_indata[6] == 0 and \
                        self.fifo_indata[7] == 0 and \
                        self.fifo_indata[8] == 0 and \
                        self.fifo_indata[9] == 0x14:
                    cmd_worked = 1
                    self.fifo_out_amt, self.fifo_outdata = \
                        pcr_extend(self.fifo_indata, self.fifo_outdata)
                if self.fifo_indata[6] == 0 and \
                        self.fifo_indata[7] == 0 and \
                        self.fifo_indata[8] == 0 and \
                        self.fifo_indata[9] == 0x15:
                    cmd_worked = 1
                    self.fifo_out_amt, self.fifo_outdata = \
                        pcr_read(self.fifo_indata, self.fifo_outdata)

        # Reset state if we had a malformed command
        if cmd_worked == 0:
            self.fifo_state = fifo_def.FIFO_IDLE
            self.fifo_in_amt = 0
            self.fifo_out_amt = 0
        # Otherwise change state to sending
        else:
            self.fifo_state = fifo_def.FIFO_SENDING


    def simulate(self, s_in):
        cmd     = s_in['cmd']
        cmdaddr = s_in['cmdaddr']
        cmddata = s_in['cmddata']
        self.s_update(s_in)

        # defaults
        dataout = 0

        # All Register Reads
        if cmd == fifo_def.RD:
            # Status Reg
            if cmdaddr == fifo_def.STS_ADDR:
                dataout = self.fifo_sts
            # Burst Reg
            if cmdaddr == fifo_def.BURST_ADDR:
                if self.fifo_state == fifo_def.FIFO_ACCEPTING:
                    dataout = fifo_def.FIFO_MAX_AMT - self.fifo_in_amt
                if self.fifo_state == fifo_def.FIFO_SENDING:
                    dataout = self.fifo_out_amt
            # Fifo Reg
            elif cmdaddr == fifo_def.FIFO_ADDR:
                if self.fifo_state == fifo_def.FIFO_SENDING:
                    # If we have data, return it and move index
                    if self.fifo_out_amt > 0:
                        dataout = self.fifo_outdata[self.fifo_out_amt]
                        self.fifo_out_amt -= 1
        # All Register Writes
        if cmd == fifo_def.WR:
            # Status Reg
            if cmdaddr == fifo_def.STS_ADDR:
                # Command is Ready to be sent
                if (cmddata == fifo_def.STS_COMMAND_READY):
                    self.fifo_state = fifo_def.FIFO_ACCEPTING
                    self.fifo_in_amt = 0
                    self.fifo_out_amt = 0
                # Tell the TPM to run a command
                elif cmddata == fifo_def.STS_GO:
                    # Run the command
                    self.runCmd()
            # Fifo Reg Write
            elif cmdaddr == fifo_def.FIFO_ADDR:
                # If the fifo isn't full update it
                if self.fifo_state != fifo_def.FIFO_FULL:
                    # If this is 5th byte its the cmd size byte
                    if (self.fifo_in_amt == 5):
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

        print self.fifo_out_amt
        return self.s_dict()

pcr_extend_cmd = [0x00, 0xC1, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
0x12, 0x13]

pcr_read_cmd = [0x00, 0xC1, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00]


if __name__ == '__main__':
    f = fifo()
    s_in = f.s_dict()

    # Send the PCR Extend Command
    for i in range(len(pcr_extend_cmd)):
        s_in['cmd'] = fifo_def.WR
        s_in['cmdaddr'] = fifo_def.FIFO_ADDR
        s_in['cmddata'] = pcr_extend_cmd[i]
        s_in['fifo_state'] = fifo_def.FIFO_ACCEPTING
        s_in = f.simulate(s_in)

    # Tell the TPM to run Command
    s_in['cmd'] = fifo_def.WR
    s_in['cmdaddr'] = fifo_def.STS_ADDR
    s_in['cmddata'] = fifo_def.STS_GO
    s_in = f.simulate(s_in)

    # Read all of the ouput
    for i in range(30):
        s_in['cmd'] = fifo_def.RD
        s_in['cmdaddr'] = fifo_def.FIFO_ADDR
        s_in['cmddata'] = 0
        s_in = f.simulate(s_in)
        print "%X" % s_in["dataout"]

    print "PCR_Extend 2"
    # Tell the TPM to ready a Command
    s_in['cmd'] = fifo_def.WR
    s_in['cmdaddr'] = fifo_def.STS_ADDR
    s_in['cmddata'] = fifo_def.STS_COMMAND_READY
    s_in = f.simulate(s_in)
    # Send the PCR Extend Command
    for i in range(len(pcr_extend_cmd)):
        s_in['cmd'] = fifo_def.WR
        s_in['cmdaddr'] = fifo_def.FIFO_ADDR
        s_in['cmddata'] = pcr_extend_cmd[i]
        s_in['fifo_state'] = fifo_def.FIFO_ACCEPTING
        s_in = f.simulate(s_in)

    # Tell the TPM to run Command
    s_in['cmd'] = fifo_def.WR
    s_in['cmdaddr'] = fifo_def.STS_ADDR
    s_in['cmddata'] = fifo_def.STS_GO
    s_in = f.simulate(s_in)

    # Read all of the ouput
    for i in range(30):
        s_in['cmd'] = fifo_def.RD
        s_in['cmdaddr'] = fifo_def.FIFO_ADDR
        s_in['cmddata'] = 0
        s_in = f.simulate(s_in)
        print "%X" % s_in["dataout"]

    #------------------------------------------
    # PCR_read
    #------------------------------------------
    print "PCR_Read"
    # Tell the TPM to ready a Command
    s_in['cmd'] = fifo_def.WR
    s_in['cmdaddr'] = fifo_def.STS_ADDR
    s_in['cmddata'] = fifo_def.STS_COMMAND_READY
    s_in = f.simulate(s_in)
    # Send the PCR Read Command
    for i in range(len(pcr_read_cmd)):
        s_in['cmd'] = fifo_def.WR
        s_in['cmdaddr'] = fifo_def.FIFO_ADDR
        s_in['cmddata'] = pcr_read_cmd[i]
        s_in['fifo_state'] = fifo_def.FIFO_ACCEPTING
        s_in = f.simulate(s_in)
    # Tell the TPM to run Command
    s_in['cmd'] = fifo_def.WR
    s_in['cmdaddr'] = fifo_def.STS_ADDR
    s_in['cmddata'] = fifo_def.STS_GO
    s_in = f.simulate(s_in)
    # Read all of the ouput
    for i in range(30):
        s_in['cmd'] = fifo_def.RD
        s_in['cmdaddr'] = fifo_def.FIFO_ADDR
        s_in['cmddata'] = 0
        s_in = f.simulate(s_in)
        print "%X" % s_in["dataout"]
#    if (s_in['fifo_sts'] & fifo_def.STS_DATA_AVAIL):
#        print "data available"
#    if (s_in['fifo_sts'] & fifo_def.STS_VALID):
#        print "data valid"
#    if (s_in['fifo_sts'] & fifo_def.STS_DATA_EXPECT):
#        print "data expected"
