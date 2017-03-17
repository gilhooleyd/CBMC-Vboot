import hashlib

pcr_data = [[0 for i in range(20)] for j in range(24)]

def pcr_extend(indata, outdata):
    CMD_SIZE = 0x1e
    print "Going to hash now"
    # TODO: use all pcr bytes not just last one
    pcr_index = indata[13]
    # Build the binary string
    binaryStr = ""
    for i in range(0, 20):
        binaryStr += chr(pcr_data[pcr_index][i])
    for i in range(14, 14+20):
        binaryStr += chr(indata[i])

    # print hashlib.sha1(binaryStr).hexdigest()

    # set the output tag
    outdata[CMD_SIZE - 1] = 0xc4
    # set the output size
    outdata[CMD_SIZE - 5] = CMD_SIZE
    out_amt    = CMD_SIZE
    # set the output data
    digest = hashlib.sha1(binaryStr).digest()
    for i in range(20):
        outdata[CMD_SIZE - 10 - i] = 0 + ord(digest[i])
        pcr_data[pcr_index][i] = 0 + ord(digest[i])

    # Return amount to send
    return out_amt, outdata

def pcr_read(indata, outdata):
    CMD_SIZE = 0x1e
    print "reading PCR"

    pcr_index = indata[13]
    # set the output tag
    outdata[CMD_SIZE - 1] = 0xc4
    # set the output size
    outdata[CMD_SIZE - 5] = CMD_SIZE
    out_amt    = CMD_SIZE
    # set the output data
    for i in range(20):
        outdata[CMD_SIZE - 10 - i] = pcr_data[pcr_index][i]

    # Return amount to send
    return out_amt, outdata

