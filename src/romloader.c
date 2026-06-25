#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include "../include/types.h"
#include "../include/romloader.h"

uint8 *get_rom_data(char *rom_path)
{
    FILE *rom = fopen(rom_path, "r");
    fseek(rom, 0, SEEK_END);
    size_t size = ftell(rom);
    rewind(rom);

    uint8 *rom_data = (uint8*) malloc(sizeof(uint8) * size);

    fread(rom_data, sizeof(uint8), size, rom);
    fclose(rom); 
    
    return rom_data;
}