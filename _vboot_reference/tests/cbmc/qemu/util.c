#include <stdint.h>

uint8_t read8(uint32_t addr) {
   uint8_t volatile *ptr = (uint8_t *) (addr + 0xFED40000);
   return *ptr;
}

void write8(uint8_t data, uint32_t addr) {
   uint8_t volatile *ptr = (uint8_t *) (addr + 0xFED40000);
   *ptr = data;
}

uint32_t read32(uint32_t addr) {
   uint32_t volatile *ptr = (uint32_t *) (addr + 0xFED40000);
   return *ptr;
}

void write32(uint32_t data, uint32_t addr) {
    uint32_t volatile * ptr = (uint32_t *) (addr + 0xFED40000);
    *ptr = data;
}

