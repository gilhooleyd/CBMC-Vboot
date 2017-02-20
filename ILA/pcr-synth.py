import ila

def updateFifo(state):
    if (state["input1"] == 1):
        state["output"] = state["input"]

    return state

def createPCRILA(synstates):
    m = ila.Abstraction("sha")

    inp = m.reg("input", 8)
    inp1 = m.reg("input1", 8)

    out = m.reg("output", 8)
    m.set_next("output", ila.choice("out_choice", [inp, out]))

    inp_dec = [inp == i for i in range(0, 512)]
    inp1_dec = [inp1 == i for i in range(0, 512)]
    out_dec =[out == i for i in range(0, 512)]
    m.decode_exprs = inp_dec + inp1_dec + out_dec

    m.synthesize("output", updateFifo)

    ast = m.get_next("output")
    m.exportOne(ast, "ast_out")


if __name__ == '__main__':
    ila.setloglevel(1, "")
    createPCRILA([])
