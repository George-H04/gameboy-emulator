#include "../include/cpu.h"
#include "../include/memory.h"

#include <stdio.h>
#include <stdlib.h>


void dispatch_block_zero(cpu_t *cpu, memory_t *mem, uint8 opcode);

void cpu_dump(cpu_t *cpu)
{
    printf("      State of CPU      \n");
    printf("*----------------------*\n");
    printf("| Register A:   0x%-4X |\n", cpu->A);
    printf("| Register F:   0x%-4X |\n", cpu->F);
    printf("| Register B:   0x%-4X |\n", cpu->B);
    printf("| Register C:   0x%-4X |\n", cpu->C);
    printf("| Register D:   0x%-4X |\n", cpu->D);
    printf("| Register E:   0x%-4X |\n", cpu->E);
    printf("| Register H:   0x%-4X |\n", cpu->H);
    printf("| Register L:   0x%-4X |\n", cpu->L);
    printf("| Register SP:  0x%-4X |\n", cpu->SP);
    printf("| Register PC:  0x%-4X |\n", cpu->PC);
    printf("*----------------------*\n");
}

uint8 fetch_8(cpu_t *cpu, memory_t *mem)
{
    return mem->data[cpu->PC++];
}

uint16 fetch_16(cpu_t *cpu, memory_t *mem)
{
    uint8 low = fetch_8(cpu, mem);
    uint8 high = fetch_8(cpu, mem);

    uint16 value = (high << 8) | low;

    return value;
}

void cpu_step(cpu_t *cpu, memory_t *mem)
{
    uint8 opcode = fetch_8(cpu, mem);

    uint8 family = (opcode & 0x3F) >> 6; // Retrieves family bits

    switch (family)
    {
        case 0:
            dispatch_block_zero(cpu, mem, opcode);
            break;
        case 1:
            ld_reg_reg(cpu, opcode);
            break;
        case 2:
        case 3:
    }
}

uint8 *get_reg_8(cpu_t *cpu, uint8 reg)
{
    switch(reg)
    {
        case 0:
            return &cpu->B;
            break;
        case 1:
            return &cpu->C;
            break;
        case 2:
            return &cpu->D;
            break;
        case 3:
            return &cpu->E;
            break;
        case 4:
            return &cpu->H;
            break;
        case 5:
            return &cpu->L;
            break;
        case 6:     //TODO: SPECIAL HALT CASE?
            return NULL;
            break;
        case 7:
            return &cpu->A;
            break;
        default:
            break;
    }
    return NULL;
}

uint16 *get_r16_reg(cpu_t *cpu, uint8 reg)
{
    switch (reg)
    {
        case 0:
            return &cpu->BC;
            break;
        case 1:
            return &cpu->DE;
            break;
        case 2:
            return &cpu->HL;
            break;
        case 3:
            return &cpu->SP;
            break;
    }
}


// HANDLE INSTRUCTIONS

void dispatch_block_zero(cpu_t *cpu, memory_t *mem, uint8 opcode)
{
    // Pattern with these functions:
    //  - If the third bit is NOT set, argument is two bits
    //       - OR IT IS SPECIFIC EDGE CASE
    //  - If third bit IS set, argument is THREE bits
    //       - OR a series of unique instructions
    
    // Clean code has one function do one thing
    //     - Can this be achieved here?
    //     - Don't prematurely optimize, can separate concerns later

    // If third bit is NOT set (technically the 2nd bit if 0-indexed)
    if ((opcode & (1 << 3)) != 1)
    {
        // Get first four and mask
        uint8 instruction = opcode & 0xF;

        switch (instruction)
        {
            case 0:     // nop
                cpu->PC++;
                break;
            case 1:     // ld r16, imm16
                uint16 imm = fetch_16(cpu, mem);
                uint8 reg = get_bits(opcode, 5, 2);
                uint16 *dest = get_r16_reg(cpu, reg);

                *dest = imm;
                break;

        }
    }

}

// Handles entirety of block 1
void ld_reg_reg(cpu_t *cpu, uint8 opcode)
{
    // MASK OUT REGISTERS
    uint8 dest_bits = (opcode & 0x38) >> 3; // BINARY 00111000
    uint8 src_bits = opcode & 0x7;   // BINARY 00000111

    uint8 *dest = get_reg_8(cpu, dest_bits);
    uint8 *src = get_reg_8(cpu, src_bits);

    if (!dest && !src)
        printf("HALT!!!\n");

    *dest = *src;
}

/**
 * Function gets bits of 'byte' starting at bit 'start'
 * and going for 'len' bits
 *
 * Function IS zero-indexed
 * 
 * len must be >= 1
 */
uint8 get_bits(uint8 byte, uint8 start, uint8 len)
{   
    return (byte >> start) & ((1u << len) - 1);
}
