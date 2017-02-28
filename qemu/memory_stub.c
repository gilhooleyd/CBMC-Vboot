#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "tty.h"

#define START_ADDR 0x00007E00

static uint32_t offset = 0;

void *VbExMalloc(size_t size) {
    uint32_t old_off = offset;
    offset += size;

//    terminal_writestring("Malloc address starts ");
//    printHex(START_ADDR + old_off);
//    terminal_writestring("\n");

    return (void *) (START_ADDR + old_off);
}

void VbExFree(void * p) {
}

int Memcmp(const void *s1, const void *s2, size_t n)
{
    unsigned char u1, u2;

    for ( ; n-- ; s1++, s2++) {
        u1 = * (unsigned char *) s1;
        u2 = * (unsigned char *) s2;
        if ( u1 != u2) {
            return (u1-u2);
        }
    }
    return 0;
}

void *Memcpy(void *dest, const void *src, uint64_t n)
{
    char *csrc = (char *)src;
    char *cdest = (char *)dest;

    // Copy contents of src[] to dest[]
    for (int i=0; i<n; i++)
        cdest[i] = csrc[i];
    return dest;
}

void *Memset(void *d, const uint8_t c, uint64_t n)
{
    uint8_t *cdest = (char *)d;

    // Copy contents of src[] to dest[]
    for (int i=0; i<n; i++)
        cdest[i] = c;
    return d;
}

