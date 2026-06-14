#include "../include/cpu.h"
#include "../include/memory.h"

#include <stdio.h>
#include <stdlib.h>

void cpu_dump(cpu_t *cpu)
{
    printf("     State of CPU      \n");
    printf("*---------------------*\n");
    printf("|Register A:  0x%-4X  |\n", cpu->A);
    printf("|Register F:  0x%-4X  |\n", cpu->F);
    printf("|Register B:  0x%-4X  |\n", cpu->B);
    printf("|Register C:  0x%-4X  |\n", cpu->C);
    printf("|Register D:  0x%-4X  |\n", cpu->D);
    printf("|Register E:  0x%-4X  |\n", cpu->E);
    printf("|Register H:  0x%-4X  |\n", cpu->H);
    printf("|Register L:  0x%-4X  |\n", cpu->L);
    printf("|Register SP: 0x%-4X  |\n", cpu->SP);
    printf("|Register PC: 0x%-4X  |\n", cpu->PC);
    printf("*---------------------*\n");

    exit(0);
}

uint8 fetch(cpu_t *cpu, memory_t *mem)
{
    return mem->data[cpu->PC];
}

void cpu_step(cpu_t *cpu, memory_t *mem)
{
    uint8 opcode = fetch(cpu, mem);

    switch (opcode)
    {
        case 0x00:      // NOP
            cpu->PC++;
            break;
        default:
            break;
    }
}