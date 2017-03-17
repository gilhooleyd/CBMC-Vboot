# ILA Directory 

This directory holds an ILA abstraction of the Trusted Platform Module.

## fifo\_sim.py

This file is a python implementation of TPM command fifo with a few 
commands implemented. 
The specification for the general registers of a TPM and the command fifo can be
found in the 
[TCG's TPM Profile Document](https://trustedcomputinggroup.org/wp-content/uploads/PC-Client-Specific-Platform-TPM-Profile-for-TPM-2-0-v43-150126.pdf). 

At the moment the following commands are implemented
* `PCR_extend`

## fifo\_synth.py

This file constructs the ILA for the TPM command fifo using `fifo_sim.py` as an
oracle. This is based on the [ILA python library](https://github.com/Bo-Yuan-Huang/ILA).

## fifo\_def.py

This file contains a list of variables needed by the TPM.
