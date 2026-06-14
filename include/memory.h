#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

typedef struct __memory_t
{
    uint8 data[1024 * 64];  // 64 KiB memory
} memory_t;

uint8 readb(memory_t *mem, uint16 addr);
void  writeb(memory_t *mem, uint16 addr, uint8 value);

#endif