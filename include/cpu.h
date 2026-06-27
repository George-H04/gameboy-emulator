#ifndef CPU_H
#define CPU_H

#include "types.h"
#include "memory.h"

typedef struct __cpu_t
{
    union {
        uint16 AF;
        struct {
            uint8 A;    // Accumulator
            uint8 F;    // Flags
        };
    };

    union {
        uint16 BC;
        struct {
            uint8 B;    
            uint8 C;
        };
    };

    union {
        uint16 DE;
        struct {
            uint8 D;    
            uint8 E;
        };
    };

    union {    // General pointer
        uint16 HL;
        struct {
            uint8 H;    
            uint8 L;
        };
    };

    uint16 SP;      // STACK POINTER
    uint16 PC;      // PROGRAM COUNTER

} cpu_t;

#define HALT        99
#define REG_ERROR   98

#define THREE_BITS  0x7

// Values are bit positions (0-indexed)
#define ZERO_FLAG   7
#define SUB_FLAG    6
#define HALF_CARRY  5
#define CARRY       4

// FUNCTION DECLARATIONS
void cpu_dump(cpu_t *cpu);

void initialize_cpu(cpu_t *cpu);

uint8 fetch_byte(cpu_t *cpu, memory_t *mem);

uint16 fetch_word(cpu_t *cpu, memory_t *mem);

void cpu_step(cpu_t *cpu, memory_t *mem);

uint8 get_bits(uint8 byte, uint8 start, uint8 len);

void dispatch_block_zero(cpu_t *cpu, memory_t *mem, uint8 opcode);

void dispatch_block_one(cpu_t *cpu, uint8 opcode, memory_t *mem);

void dispatch_block_three(cpu_t *cpu, memory_t *mem);

void uint8_to_binary(uint8 value, char *out);

#endif