
#include "tty.h"

int kernel_main(void) {
    terminal_init();
    terminal_writestring("hello world");
    while(1) {
    };
}
