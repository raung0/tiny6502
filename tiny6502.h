#ifndef TINY6502_H
#define TINY6502_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t Memory[0x10000];

typedef struct {
  uint8_t C : 1; // Carry
  uint8_t Z : 1; // Zero
  uint8_t I : 1; // Interrupt Disable
  uint8_t D : 1; // Decimal
  uint8_t B : 1; // Break
  uint8_t V : 1; // Overflow
  uint8_t N : 1; // Negative
  uint8_t _ : 1; // Unused
} CPUFlagsStruct;

typedef union {
  CPUFlagsStruct flags;
  uint8_t reg;
} CPUFlags;

typedef struct {
  uint16_t PC;
  uint8_t SP;
  uint8_t A, X, Y;
  CPUFlags P;

  bool NMI;
  bool IRQ;

  uint8_t cycles_left;

  Memory *memory;
} CPU;

uint8_t cpu_read(CPU *cpu, uint16_t addr);
void cpu_write(CPU *cpu, uint16_t addr, uint8_t data);

uint16_t cpu_read16(CPU *cpu, uint16_t addr);
void cpu_write16(CPU *cpu, uint16_t addr, uint16_t data);

void cpu_init(CPU *cpu, Memory *mem);
void cpu_reset(CPU *cpu);
void cpu_step_cycle(CPU *cpu);

#endif // TINY6502_H
