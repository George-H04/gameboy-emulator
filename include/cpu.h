#ifndef CPU_H
#define CPU_H

#include "types.h"
#include "memory.h"

typedef struct __cpu_t
{
    uint8 A;        // ACCUMULATOR
    uint8 F;        // FLAGS

    uint8 B;
    uint8 C;

    uint8 D;
    uint8 E;

    uint8 H;
    uint8 L;

    uint16 SP;      // STACK POINTER
    uint16 PC;      // PROGRAM COUNTER
} cpu_t;

// FUNCTION DECLARATIONS
void cpu_dump(cpu_t *cpu);

uint8 fetch(cpu_t *cpu, memory_t *mem);

void cpu_step(cpu_t *cpu, memory_t *mem);



#endif