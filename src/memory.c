#include "../include/memory.h"

uint8 readb(memory_t *mem, uint16 addr)
{
    return mem->data[addr];
}

void  writeb(memory_t *mem, uint16 addr, uint8 value)
{
    mem->data[addr] = value;
}