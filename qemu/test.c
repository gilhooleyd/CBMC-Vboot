
#include "tty.h"
#include "tpm.h"

VbError_t VbExTpmInit(void);
// request
// DEBUG: 00 DEBUG: c1 DEBUG: 00 DEBUG: 00 DEBUG: 00 DEBUG: 0c DEBUG: 00 DEBUG: 00 DEBUG: 00 DEBUG: 99 DEBUG:
// DEBUG: 00 DEBUG: 01 DEBUG:

uint8_t request[] = {0x00,  0xc1,  0x00,  0x00,  0x00,  0x0c,  0x00,  0x00,  0x00,  0x99, 
 0x00,  0x01 };

// response
uint8_t response[] = { 0x00,  0xc4,  0x00,  0x00,  0x00,  0x0a,  0x00,  0x00,  0x00,  0x26};

int kernel_main(void) {
    int ret;
    uint8_t resp[64];
    uint32_t resp_len;
    terminal_init();
    terminal_writestring("hello world\n");

    ret = VbExTpmInit();
    printHex(ret);
    terminal_writestring("\n");
    resp_len = 10;
	VbExTpmSendReceive(request, 12, resp, &resp_len);

    terminal_writestring("Tpm Init\n");
    while(1) {
    };
}
