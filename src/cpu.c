#include "../include/cpu.h"
#include "../include/memory.h"

#include <stdio.h>
#include <stdlib.h>

/**************************************************************
 *                        CPU LOGIC                           *
 **************************************************************/

void initialize_cpu(cpu_t *cpu)
{
    cpu->AF = 0;
    cpu->BC = 0;
    cpu->DE = 0;
    cpu->HL = 0;
    cpu->PC = 0x100;
    cpu->SP = 0;
}

void cpu_step(cpu_t *cpu, memory_t *mem)
{
    uint8 opcode = fetch_byte(cpu, mem);

    uint8 family = (opcode & 0xC0) >> 6; // Retrieves family bits

    switch (family)
    {
        case 0:
            dispatch_block_zero(cpu, mem, opcode);
            break;
        case 1:
            dispatch_block_one(cpu, opcode, mem);
            break;
        case 2:
            break;
        case 3:
            dispatch_block_three(cpu, mem);
            break;
        default:
            {
                char binary[9];
                uint8_to_binary(opcode, binary);
                printf("Opcode: %s\n", binary);
                break;
            }
    }
}

/**************************************************************
 *                       HELPER FUNCTIONS                     *
 **************************************************************/

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

uint8 fetch_byte(cpu_t *cpu, memory_t *mem)
{
    return mem->data[cpu->PC++];
}

uint16 fetch_word(cpu_t *cpu, memory_t *mem)
{
    uint8 low = fetch_byte(cpu, mem);
    uint8 high = fetch_byte(cpu, mem);

    uint16 value = (high << 8) | low;

    return value;
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
    uint8 shift = start - len + 1;
    return (byte >> shift) & ((1u << len) - 1);
}

void set_bit(uint8 *byte, uint8 pos)
{
    *byte |= (1UL << pos);
}

void clear_bits(uint8 *byte, uint8 pos, uint8 len)
{
    uint8 shift = pos - len + 1;
    uint8 mask = ((1u << len) - 1) << shift;
    *byte &= ~mask;
}

void toggle_bit(uint8 *byte, uint8 pos)
{
    *byte ^= (1UL << pos);
}

/**************************************************************
 *                      PRIVATE FUNCTIONS                     *
 **************************************************************/

static
uint8 *get_reg_8(cpu_t *cpu, uint8 reg, memory_t *mem)
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
        case 6: {     //TODO: SPECIAL HALT CASE?
            return &mem->data[cpu->HL];
            break;
        }
        case 7:
            return &cpu->A;
            break;
        default:
            break;
    }
    return NULL;
}

static
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
        default:
            return &cpu->SP;
            break;
    }
}

static
uint16 *get_r16mem_reg(cpu_t *cpu, uint8 reg)
{
    switch (reg)
    {
        case 0:
            return &cpu->BC;
        case 1:
            return &cpu->DE;
        case 2: {
            uint16 *HL_inc = &cpu->HL;
            cpu->HL++;
            return HL_inc;
        }
        default: {
            uint16 *HL_dec = &cpu->HL;
            cpu->HL--;
            return HL_dec;
        }
    }
}

static
void write_to_mem(cpu_t *cpu, memory_t *mem, uint8 opcode)
{
    uint8 dest_bits = get_bits(opcode, 5, 2);
    uint16 *dest = get_r16mem_reg(cpu, dest_bits);

    writeb(mem, *dest, cpu->A);
}

static
void mem_to_acc(cpu_t *cpu, memory_t *mem, uint8 opcode)
{
    uint8 src_bits = get_bits(opcode, 5, 2);
    uint16 *src = get_r16mem_reg(cpu, src_bits);
    cpu->A = readb(mem, *src);
}

static 
void imm_to_mem(cpu_t *cpu, memory_t *mem, uint8 opcode)
{
    uint16 imm = fetch_word(cpu, mem);
    uint8 reg = get_bits(opcode, 5, 2);
    uint16 *dest = get_r16_reg(cpu, reg);

    *dest = imm;
}

static
void sp_to_imm(cpu_t *cpu, memory_t *mem)
{
    uint16 imm = fetch_word(cpu, mem);

    writeb(mem, imm + 1, cpu->SP & 0xFF);
    writeb(mem, imm, cpu->SP >> 8);
}

static
void inc_r16(cpu_t *cpu, uint8 opcode)
{
    uint8 arg_bits = get_bits(opcode, 5, 2);
    uint16 *reg = get_r16_reg(cpu, arg_bits);

    (*reg)++;
}

static
void dec_r16(cpu_t *cpu, uint8 opcode)
{
    uint8 arg_bits = get_bits(opcode, 5, 2);
    uint16 *reg = get_r16_reg(cpu, arg_bits);

    (*reg)--;
}

static
void add_hl(cpu_t *cpu, uint8 opcode)
{
    uint8 arg_bits = get_bits(opcode, 5, 2);
    uint16 *reg = get_r16_reg(cpu, arg_bits);

    if (((uint32) cpu->HL + *reg) > 0xFFFF)
        set_bit(&cpu->F, CARRY);
    else
        clear_bits(&cpu->F, CARRY, 1);

    if ((cpu->HL & 0xFFF) + (*reg & 0xFFF) > 0xFFF)
        set_bit(&cpu->F, HALF_CARRY);
    else
        clear_bits(&cpu->F, HALF_CARRY, 1);

    cpu->HL += *reg;

    clear_bits(&cpu->F, SUB_FLAG, 1);
}

static
void inc_r8(cpu_t *cpu, uint8 opcode, memory_t *mem)
{
    uint8 bits = get_bits(opcode, 5, 3);
    uint8 *reg = get_reg_8(cpu, bits, mem);

    (*reg)++;
}

static
void dec_r8(cpu_t *cpu, uint8 opcode, memory_t *mem)
{
    uint8 bits = get_bits(opcode, 5, 3);
    uint8 *reg = get_reg_8(cpu, bits, mem);

    (*reg)--;
}

static
void ld_imm8(cpu_t *cpu, memory_t *mem, uint8 opcode)
{
    uint8 imm = fetch_byte(cpu, mem);
    uint8 bits = get_bits(opcode, 5, 3);
    uint8 *reg = get_reg_8(cpu, bits, mem);

    *reg = imm;
}

static
void jmp_imm8(cpu_t *cpu, int8 offset)
{
    cpu->PC += offset;
}

static
void rlca(cpu_t *cpu)
{
    uint8 msb = get_bits(cpu->A, 7, 1);

    cpu->A <<= 1;

    if (msb)
    {
        cpu->A |= msb;
        set_bit(&cpu->F, CARRY);
    }
    else
    {
        clear_bits(&cpu->F, CARRY, 1);
    }

    clear_bits(&cpu->F, 7, THREE_BITS);
}

static
void rrca(cpu_t *cpu)
{
    uint8 lsb = get_bits(cpu->A, 0, 1);

    cpu->A >>= 1;

    if (lsb)
    {
        cpu->A |= (lsb << 7);
        set_bit(&cpu->F, CARRY);
    }
    else
    {
        clear_bits(&cpu->F, CARRY, 1);
    }

    clear_bits(&cpu->F, 7, THREE_BITS);
}

static
void rla(cpu_t *cpu)
{
    uint8 msb = get_bits(cpu->A, 7, 1);
    uint8 carry = get_bits(cpu->F, CARRY, 1);

    cpu->A <<= 1;

    if (carry)
        set_bit(&cpu->A, 0);
    
    if (msb)
        set_bit(&cpu->F, CARRY);
    else
        clear_bits(&cpu->F, CARRY, 1);
    
    clear_bits(&cpu->F, 7, THREE_BITS);
}

static
void rra(cpu_t *cpu)
{
    uint8 lsb = get_bits(cpu->A, 0, 1);
    uint8 carry = get_bits(cpu->F, CARRY, 1);

    cpu->A >>= 1;

    if (carry)
        set_bit(&cpu->A, 7);
    
    if (lsb)
        set_bit(&cpu->F, CARRY);
    else
        clear_bits(&cpu->F, CARRY, 1);
    
    clear_bits(&cpu->F, 7, THREE_BITS);
}

static
void daa(cpu_t *cpu)
{
    uint8 adjustment = 0;
    uint8 half_carry = get_bits(cpu->F, HALF_CARRY, 1);
    uint8 carry = get_bits(cpu->F, CARRY, 1);

    if (get_bits(cpu->F, SUB_FLAG, 1))     // Subtract flag
    {
        if (half_carry) adjustment += 0x6;

        if (carry) adjustment += 0x60;

        cpu->A -= adjustment;

    }
    else
    {
        if (half_carry || (cpu->A & 0xF) > 0x9)
            adjustment += 0x6;
        
        if (carry || (cpu->A > 0x99))
        {
            adjustment += 0x60;
            set_bit(&cpu->F, CARRY);
        }

        cpu->A += adjustment;
    }

    if (cpu->A == 0)
        set_bit(&cpu->F, ZERO_FLAG);
    else
        clear_bits(&cpu->F, ZERO_FLAG, 1);

    clear_bits(&cpu->F, HALF_CARRY, 1);
}

static
void cpl(cpu_t *cpu)
{
    cpu->A = ~cpu->A;

    set_bit(&cpu->F, SUB_FLAG);
    set_bit(&cpu->F, HALF_CARRY);
}

static
void scf(cpu_t *cpu)
{
    set_bit(&cpu->F, CARRY);

    clear_bits(&cpu->F, 6, 2);
}

static
void ccf(cpu_t *cpu)
{
    toggle_bit(&cpu->F, CARRY);
    clear_bits(&cpu->F, 6, 2);
}

// HANDLE INSTRUCTIONS
void dispatch_block_zero(cpu_t *cpu, memory_t *mem, uint8 opcode)
{
    // Handle unique instructions first (and both jumps)
    switch (opcode)
    {
        case 0:     // nop
            return;
        case 7:     // rlca
            rlca(cpu);
            return;
        case 15:    // rrca
            rrca(cpu);
            return;
        case 23:    // rla
            rla(cpu);
            return;
        case 24:    // jmp, imm8
            jmp_imm8(cpu, (int8) fetch_byte(cpu, mem));
            return;
        case 31:    // rra
            rra(cpu);
            return;
        case 39:    // daa
            daa(cpu);
            return;
        case 47:    // cpl
            cpl(cpu);
            return;
        case 55:    // scf
            scf(cpu);
            return;
        case 63:    // ccf
            ccf(cpu);
            return;
        case 16:    // stop
            fetch_byte(cpu, mem);
            // Do something to mimic low power mode?
            return;
    }

    if (get_bits(opcode, 2, 3) == 0 && get_bits(opcode, 5, 1))
    {
        // Conditional Jump
        uint8 condition = get_bits(opcode, 4, 2);

        int jump = FALSE;
        switch (condition)
        {
            case 0: // NZ
                jump = (get_bits(cpu->F, 7, 1) == 0);
                break;
            case 1: // Z
                jump = (get_bits(cpu->F, 7, 1) == 1);
                break;
            case 2: // NC
                jump = (get_bits(cpu->F, 4, 1) == 0);
                break;
            case 3: // C
                jump = (get_bits(cpu->F, 4, 1) == 1);
                break;
        }

        int8 offset = (int8) fetch_byte(cpu, mem);
        if (jump)
            jmp_imm8(cpu, offset);
        
        return;
    }

    if (!(opcode & (1 << 2)))
    {
        // Get first four and mask
        uint8 instruction = opcode & 0xF;

        switch (instruction)
        {
            case 1:     // ld r16, imm16
                imm_to_mem(cpu, mem, opcode);
                break;
            case 2:     // ld [r16mem], a
                write_to_mem(cpu, mem, opcode);
                break;
            case 3:
                inc_r16(cpu, opcode);
                break;
            case 8:     // ld [imm16], sp
                sp_to_imm(cpu, mem);
                break;
            case 9:     // add hl, r16
                add_hl(cpu, opcode);
                break;
            case 10:    // ld a, [r16mem], a
                mem_to_acc(cpu, mem, opcode);
                break;
            case 11:
                dec_r16(cpu, opcode);
                break;
            default:
            {
                char binary[9];
                uint8_to_binary(opcode, binary);
                printf("Opcode: %s\n", binary);
                break;
            }
        }
    }
    else
    {
        // Get first three bits
        uint8 instruction = opcode & 0x7;

        switch (instruction)
        {
            case 4: // inc r8
                inc_r8(cpu, opcode, mem);
                break;
            case 5: // dec r8
                dec_r8(cpu, opcode, mem);
                break;
            case 6: // ld r8, imm8
                ld_imm8(cpu, mem, opcode);
                break;

        }
    }

}

// Handles entirety of block 1
void dispatch_block_one(cpu_t *cpu, uint8 opcode, memory_t *mem)
{
    // CHECK IF HALT:
    if (opcode == 0x76)
    {
        printf("HALT ENCOUNTERED!!!\n");
        return;
    }

    // MASK OUT REGISTERS
    uint8 dest_bits = (opcode & 0x38) >> 3; // BINARY 00111000
    uint8 src_bits = opcode & 0x7;   // BINARY 00000111

    uint8 *dest = get_reg_8(cpu, dest_bits, mem);
    uint8 *src = get_reg_8(cpu, src_bits, mem);

    *dest = *src;
}

void dispatch_block_three(cpu_t *cpu, memory_t *mem)
{
    // Instructions either take one operand, or one and a bit index
    uint8 opcode = fetch_byte(cpu, mem);

    // uint8 operand = get_bits(opcode, 2, 3);

    uint8 op_type = get_bits(opcode, 7, 2);

    if (op_type)
    {
        // uint8 bit_idx = get_bits(opcode, 5, 3);

        char binary[9];
        uint8_to_binary(opcode, binary);
        printf("Opcode: %s\n", binary);
    }
    else
    {
        char binary[9];
        uint8_to_binary(opcode, binary);
        printf("Opcode: %s\n", binary);
    }
}

void uint8_to_binary(uint8 value, char *out)
{
    for (int i = 7; i >= 0; i--)
    {
        out[7 - i] = ((value >> i) & 1) ? '1' : '0';
    }
    out[8] = '\0';
}
