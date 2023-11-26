#ifndef TINY6502_OPS_H
#define TINY6502_OPS_H

#include "tiny6502.h"
#include <tcl.h>

typedef enum {
  ACC,
  IMM,
  ZP,
  ZPX,
  ZPY,
  ABS,
  ABSX,
  ABSY,
  IND,
  INDX,
  INDY,
  REL,
  IMP,
} AddressingMode;

typedef void (*Instruction)(CPU *cpu, AddressingMode addr_mode);

// Instructions
void cpu_op_adc(CPU *cpu, AddressingMode addr);
void cpu_op_and(CPU *cpu, AddressingMode addr);
void cpu_op_asl(CPU *cpu, AddressingMode addr);
void cpu_op_bcc(CPU *cpu, AddressingMode addr);
void cpu_op_bcs(CPU *cpu, AddressingMode addr);
void cpu_op_beq(CPU *cpu, AddressingMode addr);
void cpu_op_bit(CPU *cpu, AddressingMode addr);
void cpu_op_bmi(CPU *cpu, AddressingMode addr);
void cpu_op_bne(CPU *cpu, AddressingMode addr);
void cpu_op_bpl(CPU *cpu, AddressingMode addr);
void cpu_op_brk(CPU *cpu, AddressingMode addr);
void cpu_op_bvc(CPU *cpu, AddressingMode addr);
void cpu_op_bvs(CPU *cpu, AddressingMode addr);
void cpu_op_clc(CPU *cpu, AddressingMode addr);
void cpu_op_cld(CPU *cpu, AddressingMode addr);
void cpu_op_cli(CPU *cpu, AddressingMode addr);
void cpu_op_clv(CPU *cpu, AddressingMode addr);
void cpu_op_cmp(CPU *cpu, AddressingMode addr);
void cpu_op_cpx(CPU *cpu, AddressingMode addr);
void cpu_op_cpy(CPU *cpu, AddressingMode addr);
void cpu_op_dec(CPU *cpu, AddressingMode addr);
void cpu_op_dex(CPU *cpu, AddressingMode addr);
void cpu_op_dey(CPU *cpu, AddressingMode addr);
void cpu_op_eor(CPU *cpu, AddressingMode addr);
void cpu_op_inc(CPU *cpu, AddressingMode addr);
void cpu_op_inx(CPU *cpu, AddressingMode addr);
void cpu_op_iny(CPU *cpu, AddressingMode addr);
void cpu_op_jmp(CPU *cpu, AddressingMode addr);
void cpu_op_jsr(CPU *cpu, AddressingMode addr);
void cpu_op_lda(CPU *cpu, AddressingMode addr);
void cpu_op_ldx(CPU *cpu, AddressingMode addr);
void cpu_op_ldy(CPU *cpu, AddressingMode addr);
void cpu_op_lsr(CPU *cpu, AddressingMode addr);
void cpu_op_nop(CPU *cpu, AddressingMode addr);
void cpu_op_ora(CPU *cpu, AddressingMode addr);
void cpu_op_pha(CPU *cpu, AddressingMode addr);
void cpu_op_php(CPU *cpu, AddressingMode addr);
void cpu_op_pla(CPU *cpu, AddressingMode addr);
void cpu_op_plp(CPU *cpu, AddressingMode addr);
void cpu_op_rol(CPU *cpu, AddressingMode addr);
void cpu_op_ror(CPU *cpu, AddressingMode addr);
void cpu_op_rti(CPU *cpu, AddressingMode addr);
void cpu_op_rts(CPU *cpu, AddressingMode addr);
void cpu_op_sbc(CPU *cpu, AddressingMode addr);
void cpu_op_sec(CPU *cpu, AddressingMode addr);
void cpu_op_sed(CPU *cpu, AddressingMode addr);
void cpu_op_sei(CPU *cpu, AddressingMode addr);
void cpu_op_sta(CPU *cpu, AddressingMode addr);
void cpu_op_stx(CPU *cpu, AddressingMode addr);
void cpu_op_sty(CPU *cpu, AddressingMode addr);
void cpu_op_tax(CPU *cpu, AddressingMode addr);
void cpu_op_tay(CPU *cpu, AddressingMode addr);
void cpu_op_tsx(CPU *cpu, AddressingMode addr);
void cpu_op_txa(CPU *cpu, AddressingMode addr);
void cpu_op_txs(CPU *cpu, AddressingMode addr);
void cpu_op_tya(CPU *cpu, AddressingMode addr);

// Illegal instructions
void cpu_op_illegal(CPU *cpu, AddressingMode addr);

// Opcode table
extern Instruction cpu_opcodes[256];
extern AddressingMode cpu_addressing_modes[256];
extern uint8_t cpu_opcode_cycles[256];
extern uint8_t cpu_opcode_page_cycles[256];

#endif // TINY6502_OPS_H
