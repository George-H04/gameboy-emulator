/**
 * SIMPLE TEST FILE FOR THE FIRST CPU INSTRUCTION
 * SHOULD JUST INCREMENT PC
 */

 #include "../include/memory.h"
 #include "../include/cpu.h"

 #include <stdlib.h>

int main(void)
{
    memory_t *mem = malloc(sizeof(memory_t));
    mem->data[0] = 0x00;
    mem->data[1] = 0x00;
    mem->data[2] = 0x00;
    mem->data[3] = 0x00;

    cpu_t *cpu = malloc(sizeof(cpu_t));
    cpu->PC = 0;

    for (int i = 0; i < 500; i++)
        cpu_step(cpu, mem);

    cpu_dump(cpu);

    exit(0);
}