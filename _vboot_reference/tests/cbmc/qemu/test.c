
#include "tty.h"
#include "tpm.h"

VbError_t VbExTpmInit(void);

int kernel_main(void) {
    int ret;
    terminal_init();
    terminal_writestring("hello world\n");
    ret = VbExTpmInit();
    printHex(ret);
    terminal_writestring("\n");
    terminal_writestring("Tpm Init\n");
    while(1) {
    };
}
