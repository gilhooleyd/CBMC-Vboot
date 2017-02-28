import ila
from fifo_sim import fifo

ADDR = 0x0
STS_ADDR = 0x24 + ADDR
FIFO_ADDR = 0x18 + ADDR
BURST_ADDR = 0x1 + ADDR

def createFifoILA():
    m = ila.Abstraction("fifo")

    # Constant values
    ZERO = m.const(0x0, 8)
    ONE = m.const(0x1, 8)

    # input data register
    cmd     = m.inp("cmd", 3)
    cmdaddr = m.inp("cmdaddr", 64)
    cmddata = m.inp("cmddata", 8)

    # TODO: Status register
    fifo_sts = m.reg("fifo_sts", 8)
    m.set_next("fifo_sts", ZERO)

    fifo_state = m.reg("fifo_state", 8)
    m.set_next("fifo_state", ila.choice("fifo_state_choice", [ZERO, ONE, ONE+1, ONE+2, ONE+3]))

    # internal index to the FIFO,
    # is amount written so far
    fifo_in_amt  = m.reg("fifo_in_amt", 8)
    m.set_next("fifo_in_amt", ila.choice("fifo_in_amt_choice", [fifo_in_amt, fifo_in_amt+1, ZERO]))

    fifo_in_cmdsize = m.reg("fifo_in_cmdsize", 8)
    m.set_next("fifo_in_cmdsize", ila.choice("fifo_in_cmdsize_choice", [fifo_in_cmdsize, cmddata, ZERO]))

    # internal index to the FIFO,
    # TODO base this size off of list of command sizes
    fifo_out_amt = m.reg("fifo_out_amt", 8)
    m.set_next("fifo_out_amt", ila.choice("fifo_out_amt_choice", [fifo_out_amt, fifo_out_amt-1, ZERO]))

    # the fifo memory.
    # 126 8 bit registers
    fifo_indata = m.mem("fifo_indata", 8, 8)
    m.set_next("fifo_indata",
            ila.choice("fifo_indata",
                [fifo_indata,
                    ila.store(fifo_indata, fifo_in_amt, cmddata)]))

    fifo_outdata = m.mem("fifo_outdata", 8, 8)
    m.set_next("fifo_outdata",
                fifo_outdata)

    # decoding is writing values to data
    addresses = [STS_ADDR, FIFO_ADDR, BURST_ADDR]
    exp = [(cmdaddr == a) & (cmd == c) & (fifo_state == s)
            for a in addresses for c in [0,1,2] for s in range(5)]
    m.decode_exprs = exp

    f = fifo()
    sim = lambda s: f.simulate(s)
    m.synthesize("fifo_in_amt", sim)
    ast = m.get_next("fifo_in_amt", 8)
    m.exportOne(ast, "fifo_in_amt.ast")

    m.synthesize("fifo_sts", sim)
    ast = m.get_next("fifo_sts", 8)
    m.exportOne(ast, "fifo_sts.ast")

if __name__ == '__main__':
    # ila.setloglevel(1, "")
    createFifoILA()
