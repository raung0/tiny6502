#include "tiny6502.h"

#include <stdint.h>
#include <string.h>

#include "tiny6502_ops.h"

void fill_opcodes(CPU *cpu);
void fill_opcode_names();

uint8_t cpu_read(CPU *cpu, uint16_t addr) { return (*cpu->memory)[addr]; }

void cpu_write(CPU *cpu, uint16_t addr, uint8_t value) {
  (*cpu->memory)[addr] = value;
}

void cpu_init(CPU *cpu, Memory *mem) {
  cpu->memory = mem;
  cpu->PC = 0;
  cpu->SP = 0;
  cpu->A = 0;
  cpu->X = 0;
  cpu->Y = 0;
  cpu->P.reg = 0;
  cpu->cycles_left = 0;

  cpu_reset(cpu);

  fill_opcodes(cpu);
  fill_opcode_names();
}

void cpu_reset(CPU *cpu) {
  cpu->PC = cpu_read(cpu, 0xFFFC) | (cpu_read(cpu, 0xFFFD) << 8);
}

void cpu_push(CPU *cpu, uint8_t value) {
  cpu_write(cpu, 0x0100 + cpu->SP, value);
  cpu->SP--;
}

uint8_t cpu_pop(CPU *cpu) {
  cpu->SP++;
  return cpu_read(cpu, 0x0100 + cpu->SP);
}

void cpu_push_state(CPU *cpu) {
  cpu_push(cpu, cpu->PC >> 8);
  cpu_push(cpu, cpu->PC & 0xFF);
  cpu_push(cpu, cpu->P.reg);
  cpu->P.flags.I = 1;
  cpu->PC = cpu_read(cpu, 0xFFFA) | (cpu_read(cpu, 0xFFFB) << 8);
}

char const *cpu_opcode_names[256] = {""};

void cpu_step_cycle(CPU *cpu) {
  if (cpu->cycles_left) {
    cpu->cycles_left--;
    return;
  }

  if (cpu->NMI) {
    cpu->NMI = 0;
    cpu_push_state(cpu);
    return;
  }

  if (cpu->IRQ && !cpu->P.flags.I) {
    cpu->IRQ = 0;
    cpu_push_state(cpu);
    return;
  }

  uint8_t opcode = cpu_read(cpu, cpu->PC++);
  cpu->cycles_left = cpu_opcode_cycles[opcode];
  printf("Executing opcode %s\n", cpu_opcode_names[opcode]);
  cpu_opcodes[opcode](cpu, cpu_addressing_modes[opcode]);
  if (cpu_opcode_page_cycles[opcode]) {
    cpu->cycles_left += cpu_opcode_page_cycles[opcode];
  }
}

Instruction cpu_opcodes[256] = {cpu_op_illegal};
AddressingMode cpu_addressing_modes[256];
uint8_t cpu_opcode_cycles[256] = {0};
uint8_t cpu_opcode_page_cycles[256] = {0};

void fill_opcode_names() {
  cpu_opcode_names[0x00] = "BRK(IMP)";
  cpu_opcode_names[0x01] = "ORA(INDX)";
  cpu_opcode_names[0x05] = "ORA(ZP)";
  cpu_opcode_names[0x06] = "ASL(ZP)";
  cpu_opcode_names[0x08] = "PHP(IMP)";
  cpu_opcode_names[0x09] = "ORA(IMM)";
  cpu_opcode_names[0x0A] = "ASL(ACC)";
  cpu_opcode_names[0x0D] = "ORA(ABS)";
  cpu_opcode_names[0x0E] = "ASL(ABS)";
  cpu_opcode_names[0x10] = "BPL(REL)";
  cpu_opcode_names[0x11] = "ORA(INDY)";
  cpu_opcode_names[0x15] = "ORA(ZPX)";
  cpu_opcode_names[0x16] = "ASL(ZPX)";
  cpu_opcode_names[0x18] = "CLC(IMP)";
  cpu_opcode_names[0x19] = "ORA(ABSY)";
  cpu_opcode_names[0x1D] = "ORA(ABSX)";
  cpu_opcode_names[0x1E] = "ASL(ABSX)";
  cpu_opcode_names[0x20] = "JSR(ABS)";
  cpu_opcode_names[0x21] = "AND(INDX)";
  cpu_opcode_names[0x24] = "BIT(ZP)";
  cpu_opcode_names[0x25] = "AND(ZP)";
  cpu_opcode_names[0x26] = "ROL(ZP)";
  cpu_opcode_names[0x28] = "PLP(IMP)";
  cpu_opcode_names[0x29] = "AND(IMM)";
  cpu_opcode_names[0x2A] = "ROL(ACC)";
  cpu_opcode_names[0x2C] = "BIT(ABS)";
  cpu_opcode_names[0x2D] = "AND(ABS)";
  cpu_opcode_names[0x2E] = "ROL(ABS)";
  cpu_opcode_names[0x30] = "BMI(REL)";
  cpu_opcode_names[0x31] = "AND(INDY)";
  cpu_opcode_names[0x35] = "AND(ZPX)";
  cpu_opcode_names[0x36] = "ROL(ZPX)";
  cpu_opcode_names[0x38] = "SEC(IMP)";
  cpu_opcode_names[0x39] = "AND(ABSY)";
  cpu_opcode_names[0x3D] = "AND(ABSX)";
  cpu_opcode_names[0x3E] = "ROL(ABSX)";
  cpu_opcode_names[0x40] = "RTI(IMP)";
  cpu_opcode_names[0x41] = "EOR(INDX)";
  cpu_opcode_names[0x45] = "EOR(ZP)";
  cpu_opcode_names[0x46] = "LSR(ZP)";
  cpu_opcode_names[0x48] = "PHA(IMP)";
  cpu_opcode_names[0x49] = "EOR(IMM)";
  cpu_opcode_names[0x4A] = "LSR(ACC)";
  cpu_opcode_names[0x4C] = "JMP(ABS)";
  cpu_opcode_names[0x4D] = "EOR(ABS)";
  cpu_opcode_names[0x4E] = "LSR(ABS)";
  cpu_opcode_names[0x50] = "BVC(REL)";
  cpu_opcode_names[0x51] = "EOR(INDY)";
  cpu_opcode_names[0x55] = "EOR(ZPX)";
  cpu_opcode_names[0x56] = "LSR(ZPX)";
  cpu_opcode_names[0x58] = "CLI(IMP)";
  cpu_opcode_names[0x59] = "EOR(ABSY)";
  cpu_opcode_names[0x5D] = "EOR(ABSX)";
  cpu_opcode_names[0x5E] = "LSR(ABSX)";
  cpu_opcode_names[0x60] = "RTS(IMP)";
  cpu_opcode_names[0x61] = "ADC(INDX)";
  cpu_opcode_names[0x65] = "ADC(ZP)";
  cpu_opcode_names[0x66] = "ROR(ZP)";
  cpu_opcode_names[0x68] = "PLA(IMP)";
  cpu_opcode_names[0x69] = "ADC(IMM)";
  cpu_opcode_names[0x6A] = "ROR(ACC)";
  cpu_opcode_names[0x6C] = "JMP(IND)";
  cpu_opcode_names[0x6D] = "ADC(ABS)";
  cpu_opcode_names[0x6E] = "ROR(ABS)";
  cpu_opcode_names[0x70] = "BVS(REL)";
  cpu_opcode_names[0x71] = "ADC(INDY)";
  cpu_opcode_names[0x75] = "ADC(ZPX)";
  cpu_opcode_names[0x76] = "ROR(ZPX)";
  cpu_opcode_names[0x78] = "SEI(IMP)";
  cpu_opcode_names[0x79] = "ADC(ABSY)";
  cpu_opcode_names[0x7D] = "ADC(ABSX)";
  cpu_opcode_names[0x7E] = "ROR(ABSX)";
  cpu_opcode_names[0x81] = "STA(INDX)";
  cpu_opcode_names[0x84] = "STY(ZP)";
  cpu_opcode_names[0x85] = "STA(ZP)";
  cpu_opcode_names[0x86] = "STX(ZP)";
  cpu_opcode_names[0x88] = "DEY(IMP)";
  cpu_opcode_names[0x8A] = "TXA(IMP)";
  cpu_opcode_names[0x8C] = "STY(ABS)";
  cpu_opcode_names[0x8D] = "STA(ABS)";
  cpu_opcode_names[0x8E] = "STX(ABS)";
  cpu_opcode_names[0x90] = "BCC(REL)";
  cpu_opcode_names[0x91] = "STA(INDY)";
  cpu_opcode_names[0x94] = "STY(ZPX)";
  cpu_opcode_names[0x95] = "STA(ZPX)";
  cpu_opcode_names[0x96] = "STX(ZPY)";
  cpu_opcode_names[0x98] = "TYA(IMP)";
  cpu_opcode_names[0x99] = "STA(ABSY)";
  cpu_opcode_names[0x9A] = "TXS(IMP)";
  cpu_opcode_names[0x9D] = "STA(ABSX)";
  cpu_opcode_names[0xA0] = "LDY(IMM)";
  cpu_opcode_names[0xA1] = "LDA(INDX)";
  cpu_opcode_names[0xA2] = "LDX(IMM)";
  cpu_opcode_names[0xA4] = "LDY(ZP)";
  cpu_opcode_names[0xA5] = "LDA(ZP)";
  cpu_opcode_names[0xA6] = "LDX(ZP)";
  cpu_opcode_names[0xA8] = "TAY(IMP)";
  cpu_opcode_names[0xA9] = "LDA(IMM)";
  cpu_opcode_names[0xAA] = "TAX(IMP)";
  cpu_opcode_names[0xAC] = "LDY(ABS)";
  cpu_opcode_names[0xAD] = "LDA(ABS)";
  cpu_opcode_names[0xAE] = "LDX(ABS)";
  cpu_opcode_names[0xB0] = "BCS(REL)";
  cpu_opcode_names[0xB1] = "LDA(INDY)";
  cpu_opcode_names[0xB4] = "LDY(ZPX)";
  cpu_opcode_names[0xB5] = "LDA(ZPX)";
  cpu_opcode_names[0xB6] = "LDX(ZPY)";
  cpu_opcode_names[0xB8] = "CLV(IMP)";
  cpu_opcode_names[0xB9] = "LDA(ABSY)";
  cpu_opcode_names[0xBA] = "TSX(IMP)";
  cpu_opcode_names[0xBC] = "LDY(ABSX)";
  cpu_opcode_names[0xBD] = "LDA(ABSX)";
  cpu_opcode_names[0xBE] = "LDX(ABSY)";
  cpu_opcode_names[0xC0] = "CPY(IMM)";
  cpu_opcode_names[0xC1] = "CMP(INDX)";
  cpu_opcode_names[0xC4] = "CPY(ZP)";
  cpu_opcode_names[0xC5] = "CMP(ZP)";
  cpu_opcode_names[0xC6] = "DEC(ZP)";
  cpu_opcode_names[0xC8] = "INY(IMP)";
  cpu_opcode_names[0xC9] = "CMP(IMM)";
  cpu_opcode_names[0xCA] = "DEX(IMP)";
  cpu_opcode_names[0xCC] = "CPY(ABS)";
  cpu_opcode_names[0xCD] = "CMP(ABS)";
  cpu_opcode_names[0xCE] = "DEC(ABS)";
  cpu_opcode_names[0xD0] = "BNE(REL)";
  cpu_opcode_names[0xD1] = "CMP(INDY)";
  cpu_opcode_names[0xD5] = "CMP(ZPX)";
  cpu_opcode_names[0xD6] = "DEC(ZPX)";
  cpu_opcode_names[0xD8] = "CLD(IMP)";
  cpu_opcode_names[0xD9] = "CMP(ABSY)";
  cpu_opcode_names[0xDD] = "CMP(ABSX)";
  cpu_opcode_names[0xDE] = "DEC(ABSX)";
  cpu_opcode_names[0xE0] = "CPX(IMM)";
  cpu_opcode_names[0xE1] = "SBC(INDX)";
  cpu_opcode_names[0xE4] = "CPX(ZP)";
  cpu_opcode_names[0xE5] = "SBC(ZP)";
  cpu_opcode_names[0xE6] = "INC(ZP)";
  cpu_opcode_names[0xE8] = "INX(IMP)";
  cpu_opcode_names[0xE9] = "SBC(IMM)";
  cpu_opcode_names[0xEA] = "NOP(IMP)";
  cpu_opcode_names[0xEC] = "CPX(ABS)";
  cpu_opcode_names[0xED] = "SBC(ABS)";
  cpu_opcode_names[0xEE] = "INC(ABS)";
  cpu_opcode_names[0xF0] = "BEQ(REL)";
  cpu_opcode_names[0xF1] = "SBC(INDY)";
  cpu_opcode_names[0xF5] = "SBC(ZPX)";
  cpu_opcode_names[0xF6] = "INC(ZPX)";
  cpu_opcode_names[0xF8] = "SED(IMP)";
  cpu_opcode_names[0xF9] = "SBC(ABSY)";
  cpu_opcode_names[0xFD] = "SBC(ABSX)";
  cpu_opcode_names[0xFE] = "INC(ABSX)";
}

void fill_opcodes(CPU *cpu) {
  // ADC
  cpu_opcodes[0x69] = cpu_op_adc;
  cpu_addressing_modes[0x69] = IMM;
  cpu_opcode_cycles[0x69] = 2;

  cpu_opcodes[0x65] = cpu_op_adc;
  cpu_addressing_modes[0x65] = ZP;
  cpu_opcode_cycles[0x65] = 3;

  cpu_opcodes[0x75] = cpu_op_adc;
  cpu_addressing_modes[0x75] = ZPX;
  cpu_opcode_cycles[0x75] = 4;

  cpu_opcodes[0x6D] = cpu_op_adc;
  cpu_addressing_modes[0x6D] = ABS;
  cpu_opcode_cycles[0x6D] = 4;

  cpu_opcodes[0x7D] = cpu_op_adc;
  cpu_addressing_modes[0x7D] = ABSX;
  cpu_opcode_cycles[0x7D] = 4;
  cpu_opcode_page_cycles[0x7D] = 1;

  cpu_opcodes[0x79] = cpu_op_adc;
  cpu_addressing_modes[0x79] = ABSY;
  cpu_opcode_cycles[0x79] = 4;
  cpu_opcode_page_cycles[0x79] = 1;

  cpu_opcodes[0x61] = cpu_op_adc;
  cpu_addressing_modes[0x61] = INDX;
  cpu_opcode_cycles[0x61] = 6;

  cpu_opcodes[0x71] = cpu_op_adc;
  cpu_addressing_modes[0x71] = INDY;
  cpu_opcode_cycles[0x71] = 5;
  cpu_opcode_page_cycles[0x71] = 1;

  // AND
  cpu_opcodes[0x29] = cpu_op_and;
  cpu_addressing_modes[0x29] = IMM;
  cpu_opcode_cycles[0x29] = 2;

  cpu_opcodes[0x25] = cpu_op_and;
  cpu_addressing_modes[0x25] = ZP;
  cpu_opcode_cycles[0x25] = 3;

  cpu_opcodes[0x35] = cpu_op_and;
  cpu_addressing_modes[0x35] = ZPX;
  cpu_opcode_cycles[0x35] = 4;

  cpu_opcodes[0x2D] = cpu_op_and;
  cpu_addressing_modes[0x2D] = ABS;
  cpu_opcode_cycles[0x2D] = 4;

  cpu_opcodes[0x3D] = cpu_op_and;
  cpu_addressing_modes[0x3D] = ABSX;
  cpu_opcode_cycles[0x3D] = 4;
  cpu_opcode_page_cycles[0x3D] = 1;

  cpu_opcodes[0x39] = cpu_op_and;
  cpu_addressing_modes[0x39] = ABSY;
  cpu_opcode_cycles[0x39] = 4;
  cpu_opcode_page_cycles[0x39] = 1;

  cpu_opcodes[0x21] = cpu_op_and;
  cpu_addressing_modes[0x21] = INDX;
  cpu_opcode_cycles[0x21] = 6;

  cpu_opcodes[0x31] = cpu_op_and;
  cpu_addressing_modes[0x31] = INDY;
  cpu_opcode_cycles[0x31] = 5;
  cpu_opcode_page_cycles[0x31] = 1;

  // ASL
  cpu_opcodes[0x0A] = cpu_op_asl;
  cpu_addressing_modes[0x0A] = ACC;
  cpu_opcode_cycles[0x0A] = 2;

  cpu_opcodes[0x06] = cpu_op_asl;
  cpu_addressing_modes[0x06] = ZP;
  cpu_opcode_cycles[0x06] = 5;

  cpu_opcodes[0x16] = cpu_op_asl;
  cpu_addressing_modes[0x16] = ZPX;
  cpu_opcode_cycles[0x16] = 6;

  cpu_opcodes[0x0E] = cpu_op_asl;
  cpu_addressing_modes[0x0E] = ABS;
  cpu_opcode_cycles[0x0E] = 6;

  cpu_opcodes[0x1E] = cpu_op_asl;
  cpu_addressing_modes[0x1E] = ABSX;
  cpu_opcode_cycles[0x1E] = 7;

  // BCC
  cpu_opcodes[0x90] = cpu_op_bcc;
  cpu_addressing_modes[0x90] = REL;
  cpu_opcode_cycles[0x90] = 2;
  cpu_opcode_page_cycles[0x90] = 2;

  // BCS
  cpu_opcodes[0xB0] = cpu_op_bcs;
  cpu_addressing_modes[0xB0] = REL;
  cpu_opcode_cycles[0xB0] = 2;
  cpu_opcode_page_cycles[0xB0] = 2;

  // BEQ
  cpu_opcodes[0xF0] = cpu_op_beq;
  cpu_addressing_modes[0xF0] = REL;
  cpu_opcode_cycles[0xF0] = 2;
  cpu_opcode_page_cycles[0xF0] = 2;

  // BIT
  cpu_opcodes[0x24] = cpu_op_bit;
  cpu_addressing_modes[0x24] = ZP;
  cpu_opcode_cycles[0x24] = 3;

  cpu_opcodes[0x2C] = cpu_op_bit;
  cpu_addressing_modes[0x2C] = ABS;
  cpu_opcode_cycles[0x2C] = 4;

  // BMI
  cpu_opcodes[0x30] = cpu_op_bmi;
  cpu_addressing_modes[0x30] = REL;
  cpu_opcode_cycles[0x30] = 2;
  cpu_opcode_page_cycles[0x30] = 2;

  // BNE
  cpu_opcodes[0xD0] = cpu_op_bne;
  cpu_addressing_modes[0xD0] = REL;
  cpu_opcode_cycles[0xD0] = 2;
  cpu_opcode_page_cycles[0xD0] = 2;

  // BPL
  cpu_opcodes[0x10] = cpu_op_bpl;
  cpu_addressing_modes[0x10] = REL;
  cpu_opcode_cycles[0x10] = 2;
  cpu_opcode_page_cycles[0x10] = 2;

  // BRK
  cpu_opcodes[0x00] = cpu_op_brk;
  cpu_addressing_modes[0x00] = IMP;
  cpu_opcode_cycles[0x00] = 7;

  // BVC
  cpu_opcodes[0x50] = cpu_op_bvc;
  cpu_addressing_modes[0x50] = REL;
  cpu_opcode_cycles[0x50] = 2;
  cpu_opcode_page_cycles[0x50] = 2;

  // BVS
  cpu_opcodes[0x70] = cpu_op_bvs;
  cpu_addressing_modes[0x70] = REL;
  cpu_opcode_cycles[0x70] = 2;
  cpu_opcode_page_cycles[0x70] = 2;

  // CLC
  cpu_opcodes[0x18] = cpu_op_clc;
  cpu_addressing_modes[0x18] = IMP;
  cpu_opcode_cycles[0x18] = 2;

  // CLD
  cpu_opcodes[0xD8] = cpu_op_cld;
  cpu_addressing_modes[0xD8] = IMP;
  cpu_opcode_cycles[0xD8] = 2;

  // CLI
  cpu_opcodes[0x58] = cpu_op_cli;
  cpu_addressing_modes[0x58] = IMP;
  cpu_opcode_cycles[0x58] = 2;

  // CLV
  cpu_opcodes[0xB8] = cpu_op_clv;
  cpu_addressing_modes[0xB8] = IMP;
  cpu_opcode_cycles[0xB8] = 2;

  // CMP
  cpu_opcodes[0xC9] = cpu_op_cmp;
  cpu_addressing_modes[0xC9] = IMM;
  cpu_opcode_cycles[0xC9] = 2;

  cpu_opcodes[0xC5] = cpu_op_cmp;
  cpu_addressing_modes[0xC5] = ZP;
  cpu_opcode_cycles[0xC5] = 3;

  cpu_opcodes[0xD5] = cpu_op_cmp;
  cpu_addressing_modes[0xD5] = ZPX;
  cpu_opcode_cycles[0xD5] = 4;

  cpu_opcodes[0xCD] = cpu_op_cmp;
  cpu_addressing_modes[0xCD] = ABS;
  cpu_opcode_cycles[0xCD] = 4;

  cpu_opcodes[0xDD] = cpu_op_cmp;
  cpu_addressing_modes[0xDD] = ABSX;
  cpu_opcode_cycles[0xDD] = 4;
  cpu_opcode_page_cycles[0xDD] = 1;

  cpu_opcodes[0xD9] = cpu_op_cmp;
  cpu_addressing_modes[0xD9] = ABSY;
  cpu_opcode_cycles[0xD9] = 4;
  cpu_opcode_page_cycles[0xD9] = 1;

  cpu_opcodes[0xC1] = cpu_op_cmp;
  cpu_addressing_modes[0xC1] = INDX;
  cpu_opcode_cycles[0xC1] = 6;

  cpu_opcodes[0xD1] = cpu_op_cmp;
  cpu_addressing_modes[0xD1] = INDY;
  cpu_opcode_cycles[0xD1] = 5;
  cpu_opcode_page_cycles[0xD1] = 1;

  // CPX
  cpu_opcodes[0xE0] = cpu_op_cpx;
  cpu_addressing_modes[0xE0] = IMM;
  cpu_opcode_cycles[0xE0] = 2;

  cpu_opcodes[0xE4] = cpu_op_cpx;
  cpu_addressing_modes[0xE4] = ZP;
  cpu_opcode_cycles[0xE4] = 3;

  cpu_opcodes[0xEC] = cpu_op_cpx;
  cpu_addressing_modes[0xEC] = ABS;
  cpu_opcode_cycles[0xEC] = 4;

  // CPY
  cpu_opcodes[0xC0] = cpu_op_cpy;
  cpu_addressing_modes[0xC0] = IMM;
  cpu_opcode_cycles[0xC0] = 2;

  cpu_opcodes[0xC4] = cpu_op_cpy;
  cpu_addressing_modes[0xC4] = ZP;
  cpu_opcode_cycles[0xC4] = 3;

  cpu_opcodes[0xCC] = cpu_op_cpy;
  cpu_addressing_modes[0xCC] = ABS;
  cpu_opcode_cycles[0xCC] = 4;

  // DEC
  cpu_opcodes[0xC6] = cpu_op_dec;
  cpu_addressing_modes[0xC6] = ZP;
  cpu_opcode_cycles[0xC6] = 5;

  cpu_opcodes[0xD6] = cpu_op_dec;
  cpu_addressing_modes[0xD6] = ZPX;
  cpu_opcode_cycles[0xD6] = 6;

  cpu_opcodes[0xCE] = cpu_op_dec;
  cpu_addressing_modes[0xCE] = ABS;
  cpu_opcode_cycles[0xCE] = 6;

  cpu_opcodes[0xDE] = cpu_op_dec;
  cpu_addressing_modes[0xDE] = ABSX;
  cpu_opcode_cycles[0xDE] = 7;

  // DEX
  cpu_opcodes[0xCA] = cpu_op_dex;
  cpu_addressing_modes[0xCA] = IMP;
  cpu_opcode_cycles[0xCA] = 2;

  // DEY
  cpu_opcodes[0x88] = cpu_op_dey;
  cpu_addressing_modes[0x88] = IMP;
  cpu_opcode_cycles[0x88] = 2;

  // EOR
  cpu_opcodes[0x49] = cpu_op_eor;
  cpu_addressing_modes[0x49] = IMM;
  cpu_opcode_cycles[0x49] = 2;

  cpu_opcodes[0x45] = cpu_op_eor;
  cpu_addressing_modes[0x45] = ZP;
  cpu_opcode_cycles[0x45] = 3;

  cpu_opcodes[0x55] = cpu_op_eor;
  cpu_addressing_modes[0x55] = ZPX;
  cpu_opcode_cycles[0x55] = 4;

  cpu_opcodes[0x4D] = cpu_op_eor;
  cpu_addressing_modes[0x4D] = ABS;
  cpu_opcode_cycles[0x4D] = 4;

  cpu_opcodes[0x5D] = cpu_op_eor;
  cpu_addressing_modes[0x5D] = ABSX;
  cpu_opcode_cycles[0x5D] = 4;
  cpu_opcode_page_cycles[0x5D] = 1;

  cpu_opcodes[0x59] = cpu_op_eor;
  cpu_addressing_modes[0x59] = ABSY;
  cpu_opcode_cycles[0x59] = 4;
  cpu_opcode_page_cycles[0x59] = 1;

  cpu_opcodes[0x41] = cpu_op_eor;
  cpu_addressing_modes[0x41] = INDX;
  cpu_opcode_cycles[0x41] = 6;

  cpu_opcodes[0x51] = cpu_op_eor;
  cpu_addressing_modes[0x51] = INDY;
  cpu_opcode_cycles[0x51] = 5;
  cpu_opcode_page_cycles[0x51] = 1;

  // INC
  cpu_opcodes[0xE6] = cpu_op_inc;
  cpu_addressing_modes[0xE6] = ZP;
  cpu_opcode_cycles[0xE6] = 5;

  cpu_opcodes[0xF6] = cpu_op_inc;
  cpu_addressing_modes[0xF6] = ZPX;
  cpu_opcode_cycles[0xF6] = 6;

  cpu_opcodes[0xEE] = cpu_op_inc;
  cpu_addressing_modes[0xEE] = ABS;
  cpu_opcode_cycles[0xEE] = 6;

  cpu_opcodes[0xFE] = cpu_op_inc;
  cpu_addressing_modes[0xFE] = ABSX;
  cpu_opcode_cycles[0xFE] = 7;

  // INX
  cpu_opcodes[0xE8] = cpu_op_inx;
  cpu_addressing_modes[0xE8] = IMP;
  cpu_opcode_cycles[0xE8] = 2;

  // INY
  cpu_opcodes[0xC8] = cpu_op_iny;
  cpu_addressing_modes[0xC8] = IMP;
  cpu_opcode_cycles[0xC8] = 2;

  // JMP
  cpu_opcodes[0x4C] = cpu_op_jmp;
  cpu_addressing_modes[0x4C] = ABS;
  cpu_opcode_cycles[0x4C] = 3;

  cpu_opcodes[0x6C] = cpu_op_jmp;
  cpu_addressing_modes[0x6C] = IND;
  cpu_opcode_cycles[0x6C] = 5;

  // JSR
  cpu_opcodes[0x20] = cpu_op_jsr;
  cpu_addressing_modes[0x20] = ABS;
  cpu_opcode_cycles[0x20] = 6;

  // LDA
  cpu_opcodes[0xA9] = cpu_op_lda;
  cpu_addressing_modes[0xA9] = IMM;
  cpu_opcode_cycles[0xA9] = 2;

  cpu_opcodes[0xA5] = cpu_op_lda;
  cpu_addressing_modes[0xA5] = ZP;
  cpu_opcode_cycles[0xA5] = 3;

  cpu_opcodes[0xB5] = cpu_op_lda;
  cpu_addressing_modes[0xB5] = ZPX;
  cpu_opcode_cycles[0xB5] = 4;

  cpu_opcodes[0xAD] = cpu_op_lda;
  cpu_addressing_modes[0xAD] = ABS;
  cpu_opcode_cycles[0xAD] = 4;

  cpu_opcodes[0xBD] = cpu_op_lda;
  cpu_addressing_modes[0xBD] = ABSX;
  cpu_opcode_cycles[0xBD] = 4;
  cpu_opcode_page_cycles[0xBD] = 1;

  cpu_opcodes[0xB9] = cpu_op_lda;
  cpu_addressing_modes[0xB9] = ABSY;
  cpu_opcode_cycles[0xB9] = 4;
  cpu_opcode_page_cycles[0xB9] = 1;

  cpu_opcodes[0xA1] = cpu_op_lda;
  cpu_addressing_modes[0xA1] = INDX;
  cpu_opcode_cycles[0xA1] = 6;

  cpu_opcodes[0xB1] = cpu_op_lda;
  cpu_addressing_modes[0xB1] = INDY;
  cpu_opcode_cycles[0xB1] = 5;
  cpu_opcode_page_cycles[0xB1] = 1;

  // LDX
  cpu_opcodes[0xA2] = cpu_op_ldx;
  cpu_addressing_modes[0xA2] = IMM;
  cpu_opcode_cycles[0xA2] = 2;

  cpu_opcodes[0xA6] = cpu_op_ldx;
  cpu_addressing_modes[0xA6] = ZP;
  cpu_opcode_cycles[0xA6] = 3;

  cpu_opcodes[0xB6] = cpu_op_ldx;
  cpu_addressing_modes[0xB6] = ZPY;
  cpu_opcode_cycles[0xB6] = 4;

  cpu_opcodes[0xAE] = cpu_op_ldx;
  cpu_addressing_modes[0xAE] = ABS;
  cpu_opcode_cycles[0xAE] = 4;

  cpu_opcodes[0xBE] = cpu_op_ldx;
  cpu_addressing_modes[0xBE] = ABSY;
  cpu_opcode_cycles[0xBE] = 4;
  cpu_opcode_page_cycles[0xBE] = 1;

  // LDY
  cpu_opcodes[0xA0] = cpu_op_ldy;
  cpu_addressing_modes[0xA0] = IMM;
  cpu_opcode_cycles[0xA0] = 2;

  cpu_opcodes[0xA4] = cpu_op_ldy;
  cpu_addressing_modes[0xA4] = ZP;
  cpu_opcode_cycles[0xA4] = 3;

  cpu_opcodes[0xB4] = cpu_op_ldy;
  cpu_addressing_modes[0xB4] = ZPX;
  cpu_opcode_cycles[0xB4] = 4;

  cpu_opcodes[0xAC] = cpu_op_ldy;
  cpu_addressing_modes[0xAC] = ABS;
  cpu_opcode_cycles[0xAC] = 4;

  cpu_opcodes[0xBC] = cpu_op_ldy;
  cpu_addressing_modes[0xBC] = ABSX;
  cpu_opcode_cycles[0xBC] = 4;

  // LSR
  cpu_opcodes[0x4A] = cpu_op_lsr;
  cpu_addressing_modes[0x4A] = ACC;
  cpu_opcode_cycles[0x4A] = 2;

  cpu_opcodes[0x46] = cpu_op_lsr;
  cpu_addressing_modes[0x46] = ZP;
  cpu_opcode_cycles[0x46] = 5;

  cpu_opcodes[0x56] = cpu_op_lsr;
  cpu_addressing_modes[0x56] = ZPX;
  cpu_opcode_cycles[0x56] = 6;

  cpu_opcodes[0x4E] = cpu_op_lsr;
  cpu_addressing_modes[0x4E] = ABS;
  cpu_opcode_cycles[0x4E] = 6;

  cpu_opcodes[0x5E] = cpu_op_lsr;
  cpu_addressing_modes[0x5E] = ABSX;
  cpu_opcode_cycles[0x5E] = 7;

  // NOP
  cpu_opcodes[0xEA] = cpu_op_nop;
  cpu_addressing_modes[0xEA] = IMP;
  cpu_opcode_cycles[0xEA] = 2;

  // ORA
  cpu_opcodes[0x09] = cpu_op_ora;
  cpu_addressing_modes[0x09] = IMM;
  cpu_opcode_cycles[0x09] = 2;

  cpu_opcodes[0x05] = cpu_op_ora;
  cpu_addressing_modes[0x05] = ZP;
  cpu_opcode_cycles[0x05] = 3;

  cpu_opcodes[0x15] = cpu_op_ora;
  cpu_addressing_modes[0x15] = ZPX;
  cpu_opcode_cycles[0x15] = 4;

  cpu_opcodes[0x0D] = cpu_op_ora;
  cpu_addressing_modes[0x0D] = ABS;
  cpu_opcode_cycles[0x0D] = 4;

  cpu_opcodes[0x1D] = cpu_op_ora;
  cpu_addressing_modes[0x1D] = ABSX;
  cpu_opcode_cycles[0x1D] = 4;
  cpu_opcode_page_cycles[0x1D] = 1;

  cpu_opcodes[0x19] = cpu_op_ora;
  cpu_addressing_modes[0x19] = ABSY;
  cpu_opcode_cycles[0x19] = 4;
  cpu_opcode_page_cycles[0x19] = 1;

  cpu_opcodes[0x01] = cpu_op_ora;
  cpu_addressing_modes[0x01] = INDX;
  cpu_opcode_cycles[0x01] = 6;

  cpu_opcodes[0x11] = cpu_op_ora;
  cpu_addressing_modes[0x11] = INDY;
  cpu_opcode_cycles[0x11] = 5;

  // PHA
  cpu_opcodes[0x48] = cpu_op_pha;
  cpu_addressing_modes[0x48] = IMP;
  cpu_opcode_cycles[0x48] = 3;

  // PHP
  cpu_opcodes[0x08] = cpu_op_php;
  cpu_addressing_modes[0x08] = IMP;
  cpu_opcode_cycles[0x08] = 3;

  // PLA
  cpu_opcodes[0x68] = cpu_op_pla;
  cpu_addressing_modes[0x68] = IMP;
  cpu_opcode_cycles[0x68] = 4;

  // PLP
  cpu_opcodes[0x28] = cpu_op_plp;
  cpu_addressing_modes[0x28] = IMP;
  cpu_opcode_cycles[0x28] = 4;

  // ROL
  cpu_opcodes[0x2A] = cpu_op_rol;
  cpu_addressing_modes[0x2A] = ACC;
  cpu_opcode_cycles[0x2A] = 2;

  cpu_opcodes[0x26] = cpu_op_rol;
  cpu_addressing_modes[0x26] = ZP;
  cpu_opcode_cycles[0x26] = 5;

  cpu_opcodes[0x36] = cpu_op_rol;
  cpu_addressing_modes[0x36] = ZPX;
  cpu_opcode_cycles[0x36] = 6;

  cpu_opcodes[0x2E] = cpu_op_rol;
  cpu_addressing_modes[0x2E] = ABS;
  cpu_opcode_cycles[0x2E] = 6;

  cpu_opcodes[0x3E] = cpu_op_rol;
  cpu_addressing_modes[0x3E] = ABSX;
  cpu_opcode_cycles[0x3E] = 7;

  // ROR
  cpu_opcodes[0x6A] = cpu_op_ror;
  cpu_addressing_modes[0x6A] = ACC;
  cpu_opcode_cycles[0x6A] = 2;

  cpu_opcodes[0x66] = cpu_op_ror;
  cpu_addressing_modes[0x66] = ZP;
  cpu_opcode_cycles[0x66] = 5;

  cpu_opcodes[0x76] = cpu_op_ror;
  cpu_addressing_modes[0x76] = ZPX;
  cpu_opcode_cycles[0x76] = 6;

  cpu_opcodes[0x6E] = cpu_op_ror;
  cpu_addressing_modes[0x6E] = ABS;
  cpu_opcode_cycles[0x6E] = 6;

  cpu_opcodes[0x7E] = cpu_op_ror;
  cpu_addressing_modes[0x7E] = ABSX;

  // RTI
  cpu_opcodes[0x40] = cpu_op_rti;
  cpu_addressing_modes[0x40] = IMP;
  cpu_opcode_cycles[0x40] = 6;

  // RTS
  cpu_opcodes[0x60] = cpu_op_rts;
  cpu_addressing_modes[0x60] = IMP;
  cpu_opcode_cycles[0x60] = 6;

  // SBC
  cpu_opcodes[0xE9] = cpu_op_sbc;
  cpu_addressing_modes[0xE9] = IMM;
  cpu_opcode_cycles[0xE9] = 2;

  cpu_opcodes[0xE5] = cpu_op_sbc;
  cpu_addressing_modes[0xE5] = ZP;
  cpu_opcode_cycles[0xE5] = 3;

  cpu_opcodes[0xF5] = cpu_op_sbc;
  cpu_addressing_modes[0xF5] = ZPX;
  cpu_opcode_cycles[0xF5] = 4;

  cpu_opcodes[0xED] = cpu_op_sbc;
  cpu_addressing_modes[0xED] = ABS;
  cpu_opcode_cycles[0xED] = 4;

  cpu_opcodes[0xFD] = cpu_op_sbc;
  cpu_addressing_modes[0xFD] = ABSX;
  cpu_opcode_cycles[0xFD] = 4;
  cpu_opcode_page_cycles[0xFD] = 1;

  cpu_opcodes[0xF9] = cpu_op_sbc;
  cpu_addressing_modes[0xF9] = ABSY;
  cpu_opcode_cycles[0xF9] = 4;
  cpu_opcode_page_cycles[0xF9] = 1;

  cpu_opcodes[0xE1] = cpu_op_sbc;
  cpu_addressing_modes[0xE1] = INDX;
  cpu_opcode_cycles[0xE1] = 6;

  cpu_opcodes[0xF1] = cpu_op_sbc;
  cpu_addressing_modes[0xF1] = INDY;
  cpu_opcode_cycles[0xF1] = 5;
  cpu_opcode_page_cycles[0xF1] = 1;

  // SEC
  cpu_opcodes[0x38] = cpu_op_sec;
  cpu_addressing_modes[0x38] = IMP;
  cpu_opcode_cycles[0x38] = 2;

  // SED
  cpu_opcodes[0xF8] = cpu_op_sed;
  cpu_addressing_modes[0xF8] = IMP;
  cpu_opcode_cycles[0xF8] = 2;

  // SEI
  cpu_opcodes[0x78] = cpu_op_sei;
  cpu_addressing_modes[0x78] = IMP;
  cpu_opcode_cycles[0x78] = 2;

  // STA
  cpu_opcodes[0x85] = cpu_op_sta;
  cpu_addressing_modes[0x85] = ZP;
  cpu_opcode_cycles[0x85] = 3;

  cpu_opcodes[0x95] = cpu_op_sta;
  cpu_addressing_modes[0x95] = ZPX;
  cpu_opcode_cycles[0x95] = 4;

  cpu_opcodes[0x8D] = cpu_op_sta;
  cpu_addressing_modes[0x8D] = ABS;
  cpu_opcode_cycles[0x8D] = 4;

  cpu_opcodes[0x9D] = cpu_op_sta;
  cpu_addressing_modes[0x9D] = ABSX;
  cpu_opcode_cycles[0x9D] = 5;

  cpu_opcodes[0x99] = cpu_op_sta;
  cpu_addressing_modes[0x99] = ABSY;
  cpu_opcode_cycles[0x99] = 5;

  cpu_opcodes[0x81] = cpu_op_sta;
  cpu_addressing_modes[0x81] = INDX;
  cpu_opcode_cycles[0x81] = 6;

  cpu_opcodes[0x91] = cpu_op_sta;
  cpu_addressing_modes[0x91] = INDY;
  cpu_opcode_cycles[0x91] = 6;

  // STX
  cpu_opcodes[0x86] = cpu_op_stx;
  cpu_addressing_modes[0x86] = ZP;
  cpu_opcode_cycles[0x86] = 3;

  cpu_opcodes[0x96] = cpu_op_stx;
  cpu_addressing_modes[0x96] = ZPY;
  cpu_opcode_cycles[0x96] = 4;

  cpu_opcodes[0x8E] = cpu_op_stx;
  cpu_addressing_modes[0x8E] = ABS;
  cpu_opcode_cycles[0x8E] = 4;

  // STY
  cpu_opcodes[0x84] = cpu_op_sty;
  cpu_addressing_modes[0x84] = ZP;
  cpu_opcode_cycles[0x84] = 3;

  cpu_opcodes[0x94] = cpu_op_sty;
  cpu_addressing_modes[0x94] = ZPX;
  cpu_opcode_cycles[0x94] = 4;

  cpu_opcodes[0x8C] = cpu_op_sty;
  cpu_addressing_modes[0x8C] = ABS;
  cpu_opcode_cycles[0x8C] = 4;

  // TAX
  cpu_opcodes[0xAA] = cpu_op_tax;
  cpu_addressing_modes[0xAA] = IMP;
  cpu_opcode_cycles[0xAA] = 2;

  // TAY
  cpu_opcodes[0xA8] = cpu_op_tay;
  cpu_addressing_modes[0xA8] = IMP;
  cpu_opcode_cycles[0xA8] = 2;

  // TSX
  cpu_opcodes[0xBA] = cpu_op_tsx;
  cpu_addressing_modes[0xBA] = IMP;
  cpu_opcode_cycles[0xBA] = 2;

  // TXA
  cpu_opcodes[0x8A] = cpu_op_txa;
  cpu_addressing_modes[0x8A] = IMP;
  cpu_opcode_cycles[0x8A] = 2;

  // TXS
  cpu_opcodes[0x9A] = cpu_op_txs;
  cpu_addressing_modes[0x9A] = IMP;
  cpu_opcode_cycles[0x9A] = 2;

  // TYA
  cpu_opcodes[0x98] = cpu_op_tya;
  cpu_addressing_modes[0x98] = IMP;
  cpu_opcode_cycles[0x98] = 2;
}
