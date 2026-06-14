#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include "../include/types.h"

int main(int argc, char **argv)
{   
    if (argc != 2)
    {
        printf("Incorrect args. Form is: emu <ROM_PATH>\n");
        exit(1);
    }

    FILE *rom = fopen(argv[1], "r");
    fseek(rom, 0, SEEK_END);
    size_t size = ftell(rom);
    rewind(rom);

    uint8 *rom_data = (uint8*) malloc(sizeof(uint8) * size);

    fread(rom_data, sizeof(uint8), size, rom);
    fclose(rom);

    for (int i = 0x134; i <= 0x141; i++)
    {
        putchar(rom_data[i]);
    }
    putchar('\n');

    exit(0);
}