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
    mem->data[0] = 0x41;


    cpu_t *cpu = malloc(sizeof(cpu_t));

    cpu->B = 0;
    cpu->C = 5;

    cpu_dump(cpu);
    cpu_step(cpu, mem);
    cpu_dump(cpu);

    exit(0);
}