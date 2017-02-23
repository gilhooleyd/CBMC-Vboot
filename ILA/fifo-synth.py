import ila


# this is the simulation of a fifo
def actualFifo(state):
    # get variables
    amt = state["AMT"]
    fifoMem = state["fifoMem"]
    data = state["DAT_R"]
    stat = state["STS_R"]

    # lets say that fifo has a length of 4
    if (amt < 4):
        # write data to fifo
        fifoMem[amt] = data
        stat = 1
        amt += 1
        if (amt == 4):
            stat = 0
    # reset fifo if we see this specific value when full
    elif (data == '0xaa'):
        amt = 0
        stat = 1

    state["AMT"] = amt
    state["fifoMem"] = fifoMem
    state["DAT_R"] = data
    state["STS_R"] = stat
    return state

def createFifoILA():
    m = ila.Abstraction("fifo")

    # Constant values
    ZERO = m.const(0x0, 8)
    ONE = m.const(0x1, 8)

    # input data register
    data = m.reg("DAT_R", 8)

    # output status register
    # (for now 1 if can write, 0 otherwise)
    stat = m.reg("STS_R", 8)
    m.set_next("STS_R", ila.choice("STS_R_choice", [ZERO, ONE]))

    # internal index to the FIFO,
    # is amount written so far
    amt  = m.reg("AMT", 8)
    m.set_next("AMT", ila.choice("AMT_choice", [amt, amt+1, ZERO]))

    # the fifo memory.
    # 256 8 bit registers
    fifoMem = m.mem("fifoMem", 8, 8)
    m.set_next("fifoMem",
            ila.choice("fifoMem",
                [fifoMem,
                    ila.store(fifoMem, amt, data)]))

    # decoding is writing values to data
    m.decode_exprs = [data == i for i in range(0, 512)]

    ast = m.get_next("STS_R", 8)
    m.exportOne(ast, "str_out")

if __name__ == '__main__':
    # ila.setloglevel(1, "")
    createFifoILA()
