#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/types.h"
#include "../include/cpu.h"
#include "../include/romloader.h"


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Incorrect args. Form is: ./emu <ROM_PATH>\n");
        exit(1);
    }

    uint8 *rom_data = get_rom_data(argv[1]);

    cpu_t cpu = {};
    initialize_cpu(&cpu);

    memory_t mem = {};
    memcpy(mem.data, rom_data, sizeof(mem.data) / 8);

    // Main execution loop
    while (cpu.PC <= sizeof(mem.data) / 8)
    {
        cpu_step(&cpu, &mem);
    }

    exit(0);
}