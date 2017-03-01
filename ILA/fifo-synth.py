import ila
from simulate import fifo

ADDR = 0xFED40000
STS_ADDR = 0x24 + ADDR
FIFO_ADDR = 0x18 + ADDR
BURST_ADDR = 0x1 + ADDR

def createFifoILA():
    m = ila.Abstraction("fifo")

    # Constant values
    ZERO = m.const(0x0, 8)
    ONE = m.const(0x1, 8)

    # input data register
    cmd     = m.inp("cmd", 2)
    cmdaddr = m.inp("cmdaddr", 8)
    cmddata = m.inp("cmdaddr", 8)

    # TODO: Status register

    # internal index to the FIFO,
    # is amount written so far
    fifo_in_amt  = m.reg("fifo_in_amt", 8)
    m.set_next("fifo_in_amt", ila.choice("fifo_in_amt_choice", [fifo_in_amt, fifo_in_amt+1, ZERO]))

    fifo_in_cmdsize = m.reg("fifo_in_cmdsize", 8)
    m.set_next("fifo_in_cmdsize", ila.choice("fifo_in_cmdsize_choice", [fifo_in_cmdsize, cmddata, 0]))

    # internal index to the FIFO,
    # TODO base this size off of list of command sizes
    fifo_out_amt = m.reg("fifo_out_amt", 8)

    m.set_next("fifo_out_amt", ila.choice("fifo_out_amt_choice", [fifo_out_amt, fifo_out_amt-1, ZERO]))

    # the fifo memory.
    # 126 8 bit registers
    fifo_indata = m.mem("fifo_indata", 8, 6)
    m.set_next("fifo_indata",
            ila.choice("fifo_indata",
                [fifoMem,
                    ila.store(fifo_indata, amt, data)]))

    # decoding is writing values to data
    addresses = [STS_ADDR, FIFO_ADDR, BURST_ADDR]
    m.decode_exprs = [(cmdaddress == a) and (cmd == c) and (fifo_state == s) for a in addresses for c in [0,1,2] for s in range(5)]

    ast = m.get_next("fifo_in_amt", 8)
    m.exportOne(ast, "in_amt.ast")

if __name__ == '__main__':
    # ila.setloglevel(1, "")
    createFifoILA()
