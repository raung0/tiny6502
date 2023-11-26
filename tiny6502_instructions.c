#include "tiny6502.h"
#include "tiny6502_ops.h"

extern void cpu_push(CPU *cpu, uint8_t value);
extern uint8_t cpu_pop(CPU *cpu);

uint16_t cpu_get_value_at_address(CPU *cpu, AddressingMode addr_mode) {
  uint16_t value = 0;
  switch (addr_mode) {
  case ACC:
    value = cpu->A;
    break;
  case IMM:
    value = (*cpu->memory)[cpu->PC++];
    break;
  case ZP:
    value = (*cpu->memory)[(*cpu->memory)[cpu->PC++]];
    break;
  case ZPX:
    value = (*cpu->memory)[(*cpu->memory)[cpu->PC++] + cpu->X];
    break;
  case ZPY:
    value = (*cpu->memory)[(*cpu->memory)[cpu->PC++] + cpu->Y];
    break;
  case ABS:
    value = (*cpu->memory)[cpu->PC++];
    value |= (*cpu->memory)[cpu->PC++] << 8;
    break;
  case ABSX:
    value = (*cpu->memory)[cpu->PC++];
    value |= (*cpu->memory)[cpu->PC++] << 8;
    value += cpu->X;
    break;
  case ABSY:
    value = (*cpu->memory)[cpu->PC++];
    value |= (*cpu->memory)[cpu->PC++] << 8;
    value += cpu->Y;
    break;
  case IND:
    value = (*cpu->memory)[cpu->PC++];
    value |= (*cpu->memory)[cpu->PC++] << 8;
    value = (*cpu->memory)[value];
    value |= (*cpu->memory)[value + 1] << 8;
    break;
  case INDX:
    value = (*cpu->memory)[cpu->PC++];
    value |= (*cpu->memory)[cpu->PC++] << 8;
    value += cpu->X;
    value = (*cpu->memory)[value];
    value |= (*cpu->memory)[value + 1] << 8;
    break;
  case INDY:
    value = (*cpu->memory)[cpu->PC++];
    value |= (*cpu->memory)[cpu->PC++] << 8;
    value = (*cpu->memory)[value];
    value |= (*cpu->memory)[value + 1] << 8;
    value += cpu->Y;
    break;
  default:
    break;
  }
  return value;
}

void cpu_op_adc(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  uint16_t result = cpu->A + value + cpu->P.flags.C;
  cpu->P.flags.C = result > 0xFF;
  cpu->P.flags.V = (uint8_t)((~(cpu->A ^ value) & (cpu->A ^ result)) & 0x80);
  cpu->A = result & 0xFF;
  cpu->P.flags.Z = cpu->A == 0;
  cpu->P.flags.N = cpu->A & 0x80;
}

void cpu_op_and(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->A &= value;
  cpu->P.flags.Z = cpu->A == 0;
  cpu->P.flags.N = cpu->A & 0x80;
}

void cpu_op_asl(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->P.flags.C = value & 0x80;
  value <<= 1;
  cpu->P.flags.Z = value == 0;
  cpu->P.flags.N = value & 0x80;
}

void cpu_op_bcc(CPU *cpu, AddressingMode addr) {
  if (!cpu->P.flags.C)
    cpu->PC += cpu_get_value_at_address(cpu, addr);
}

void cpu_op_bcs(CPU *cpu, AddressingMode addr) {
  if (cpu->P.flags.C)
    cpu->PC += cpu_get_value_at_address(cpu, addr);
}

void cpu_op_beq(CPU *cpu, AddressingMode addr) {
  if (cpu->P.flags.Z)
    cpu->PC += cpu_get_value_at_address(cpu, addr);
}

void cpu_op_bit(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->P.flags.Z = (cpu->A & value) == 0;
  cpu->P.flags.N = value & 0x80;
  cpu->P.flags.V = value & 0x40;
}

void cpu_op_bmi(CPU *cpu, AddressingMode addr) {
  if (cpu->P.flags.N)
    cpu->PC += cpu_get_value_at_address(cpu, addr);
}

void cpu_op_bne(CPU *cpu, AddressingMode addr) {
  if (!cpu->P.flags.Z)
    cpu->PC += cpu_get_value_at_address(cpu, addr);
}

void cpu_op_bpl(CPU *cpu, AddressingMode addr) {
  if (!cpu->P.flags.N)
    cpu->PC += cpu_get_value_at_address(cpu, addr);
}

void cpu_op_brk(CPU *cpu, AddressingMode addr) {
  cpu->PC++;
  cpu->P.flags.B = 1;
  cpu_push(cpu, cpu->PC >> 8);
  cpu_push(cpu, cpu->PC & 0xFF);
  cpu_push(cpu, cpu->P.reg);
  cpu->P.flags.I = 1;
  cpu->PC = (*cpu->memory)[0xFFFE];
  cpu->PC |= (*cpu->memory)[0xFFFF] << 8;
}

void cpu_op_bvc(CPU *cpu, AddressingMode addr) {
  if (!cpu->P.flags.V)
    cpu->PC += cpu_get_value_at_address(cpu, addr);
}

void cpu_op_bvs(CPU *cpu, AddressingMode addr) {
  if (cpu->P.flags.V)
    cpu->PC += cpu_get_value_at_address(cpu, addr);
}

void cpu_op_clc(CPU *cpu, AddressingMode addr) { cpu->P.flags.C = 0; }

void cpu_op_cld(CPU *cpu, AddressingMode addr) { cpu->P.flags.D = 0; }

void cpu_op_cli(CPU *cpu, AddressingMode addr) { cpu->P.flags.I = 0; }

void cpu_op_clv(CPU *cpu, AddressingMode addr) { cpu->P.flags.V = 0; }

void cpu_op_cmp(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->P.flags.C = cpu->A >= value;
  cpu->P.flags.Z = cpu->A == value;
  cpu->P.flags.N = (cpu->A - value) & 0x80;
}

void cpu_op_cpx(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->P.flags.C = cpu->X >= value;
  cpu->P.flags.Z = cpu->X == value;
  cpu->P.flags.N = (cpu->X - value) & 0x80;
}

void cpu_op_cpy(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->P.flags.C = cpu->Y >= value;
  cpu->P.flags.Z = cpu->Y == value;
  cpu->P.flags.N = (cpu->Y - value) & 0x80;
}

void cpu_op_dec(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  value--;
  cpu->P.flags.Z = value == 0;
  cpu->P.flags.N = value & 0x80;
}

void cpu_op_dex(CPU *cpu, AddressingMode addr) {
  cpu->X--;
  cpu->P.flags.Z = cpu->X == 0;
  cpu->P.flags.N = cpu->X & 0x80;
}

void cpu_op_dey(CPU *cpu, AddressingMode addr) {
  cpu->Y--;
  cpu->P.flags.Z = cpu->Y == 0;
  cpu->P.flags.N = cpu->Y & 0x80;
}

void cpu_op_eor(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->A ^= value;
  cpu->P.flags.Z = cpu->A == 0;
  cpu->P.flags.N = cpu->A & 0x80;
}

void cpu_op_inc(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  value++;
  cpu->P.flags.Z = value == 0;
  cpu->P.flags.N = value & 0x80;
}

void cpu_op_inx(CPU *cpu, AddressingMode addr) {
  cpu->X++;
  cpu->P.flags.Z = cpu->X == 0;
  cpu->P.flags.N = cpu->X & 0x80;
}

void cpu_op_iny(CPU *cpu, AddressingMode addr) {
  cpu->Y++;
  cpu->P.flags.Z = cpu->Y == 0;
  cpu->P.flags.N = cpu->Y & 0x80;
}

void cpu_op_jmp(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->PC = value;
}

void cpu_op_jsr(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->PC--;
  cpu_push(cpu, cpu->PC >> 8);
  cpu_push(cpu, cpu->PC & 0xFF);
  cpu->PC = value;
}

void cpu_op_lda(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->A = value;
  cpu->P.flags.Z = cpu->A == 0;
  cpu->P.flags.N = cpu->A & 0x80;
}

void cpu_op_ldx(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->X = value;
  cpu->P.flags.Z = cpu->X == 0;
  cpu->P.flags.N = cpu->X & 0x80;
}

void cpu_op_ldy(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->Y = value;
  cpu->P.flags.Z = cpu->Y == 0;
  cpu->P.flags.N = cpu->Y & 0x80;
}

void cpu_op_lsr(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->P.flags.C = value & 0x01;
  value >>= 1;
  cpu->P.flags.Z = value == 0;
  cpu->P.flags.N = value & 0x80;
}

void cpu_op_nop(CPU *cpu, AddressingMode addr) {
  // Do nothing
}

void cpu_op_ora(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  cpu->A |= value;
  cpu->P.flags.Z = cpu->A == 0;
  cpu->P.flags.N = cpu->A & 0x80;
}

void cpu_op_pha(CPU *cpu, AddressingMode addr) { cpu_push(cpu, cpu->A); }

void cpu_op_php(CPU *cpu, AddressingMode addr) { cpu_push(cpu, cpu->P.reg); }

void cpu_op_pla(CPU *cpu, AddressingMode addr) {
  cpu->A = (*cpu->memory)[0x0100 + cpu->SP++];
  cpu->P.flags.Z = cpu->A == 0;
  cpu->P.flags.N = cpu->A & 0x80;
}

void cpu_op_plp(CPU *cpu, AddressingMode addr) {
  cpu->P.reg = (*cpu->memory)[0x0100 + cpu->SP++];
}

void cpu_op_rol(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  uint16_t result = (value << 1) | cpu->P.flags.C;
  cpu->P.flags.C = value & 0x80;
  value = result;
  cpu->P.flags.Z = value == 0;
  cpu->P.flags.N = value & 0x80;
}

void cpu_op_ror(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  uint16_t result = (value >> 1) | (cpu->P.flags.C << 7);
  cpu->P.flags.C = value & 0x01;
  value = result;
  cpu->P.flags.Z = value == 0;
  cpu->P.flags.N = value & 0x80;
}

void cpu_op_rti(CPU *cpu, AddressingMode addr) {
  cpu->P.reg = (*cpu->memory)[0x0100 + cpu->SP++];
  cpu->PC = (*cpu->memory)[0x0100 + cpu->SP++];
  cpu->PC |= (*cpu->memory)[0x0100 + cpu->SP++] << 8;
}

void cpu_op_rts(CPU *cpu, AddressingMode addr) {
  cpu->PC = (*cpu->memory)[0x0100 + cpu->SP++];
  cpu->PC |= (*cpu->memory)[0x0100 + cpu->SP++] << 8;
  cpu->PC++;
}

void cpu_op_sbc(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  uint16_t result = cpu->A - value - (1 - cpu->P.flags.C);
  cpu->P.flags.C = result < 0x100;
  cpu->P.flags.V = ((cpu->A ^ result) & 0x80) && ((cpu->A ^ value) & 0x80);
  cpu->A = result & 0xFF;
  cpu->P.flags.Z = cpu->A == 0;
  cpu->P.flags.N = cpu->A & 0x80;
}

void cpu_op_sec(CPU *cpu, AddressingMode addr) { cpu->P.flags.C = 1; }

void cpu_op_sed(CPU *cpu, AddressingMode addr) { cpu->P.flags.D = 1; }

void cpu_op_sei(CPU *cpu, AddressingMode addr) {
  cpu->P.flags.I = 1;
  cpu->P.flags.D = 1;
}

void cpu_op_sta(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  (*cpu->memory)[value] = cpu->A;
}

void cpu_op_stx(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  (*cpu->memory)[value] = cpu->X;
}

void cpu_op_sty(CPU *cpu, AddressingMode addr) {
  uint16_t value = cpu_get_value_at_address(cpu, addr);
  (*cpu->memory)[value] = cpu->Y;
}

void cpu_op_tax(CPU *cpu, AddressingMode addr) {
  cpu->X = cpu->A;
  cpu->P.flags.Z = cpu->X == 0;
  cpu->P.flags.N = cpu->X & 0x80;
}

void cpu_op_tay(CPU *cpu, AddressingMode addr) {
  cpu->Y = cpu->A;
  cpu->P.flags.Z = cpu->Y == 0;
  cpu->P.flags.N = cpu->Y & 0x80;
}

void cpu_op_tsx(CPU *cpu, AddressingMode addr) {
  cpu->X = cpu->SP;
  cpu->P.flags.Z = cpu->X == 0;
  cpu->P.flags.N = cpu->X & 0x80;
}

void cpu_op_txa(CPU *cpu, AddressingMode addr) {
  cpu->A = cpu->X;
  cpu->P.flags.Z = cpu->A == 0;
  cpu->P.flags.N = cpu->A & 0x80;
}

void cpu_op_txs(CPU *cpu, AddressingMode addr) { cpu->SP = cpu->X; }

void cpu_op_tya(CPU *cpu, AddressingMode addr) {
  cpu->A = cpu->Y;
  cpu->P.flags.Z = cpu->A == 0;
  cpu->P.flags.N = cpu->A & 0x80;
}

void cpu_op_illegal(CPU *cpu, AddressingMode addr) {
  // Do nothing
}
