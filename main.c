#include "tiny6502.h"

#include <stdio.h>

void cpu_print_state(CPU *cpu) {
  printf("A: $%02X X: $%02X Y: $%02X SP: $%02X PC: $%04X P: %08b Cycles left: "
         "%d \n",
         cpu->A, cpu->X, cpu->Y, cpu->SP, cpu->PC, cpu->P.reg,
         cpu->cycles_left);
}

int main(void) {
  Memory memory;

  // a9 01 8d 00 02 a9 05 8d 01 02 a9 08 8d 02 02
  memory[0x0200] = 0xA9;
  memory[0x0201] = 0x01;
  memory[0x0202] = 0x8D;
  memory[0x0203] = 0x00;
  memory[0x0204] = 0x02;
  memory[0x0205] = 0xA9;
  memory[0x0206] = 0x05;
  memory[0x0207] = 0x8D;
  memory[0x0208] = 0x01;
  memory[0x0209] = 0x02;
  memory[0x020A] = 0xA9;
  memory[0x020B] = 0x08;
  memory[0x020C] = 0x8D;
  memory[0x020D] = 0x02;
  memory[0x020E] = 0x02;

  memory[0xFFFC] = 0x00;
  memory[0xFFFD] = 0x02;

  CPU cpu;
  cpu_init(&cpu, &memory);
  cpu_print_state(&cpu);

  while (cpu.PC < 0x020F) {
    getchar();
    do
      cpu_step_cycle(&cpu);
    while (cpu.cycles_left > 0);
    cpu_print_state(&cpu);
  }

  puts("Done! Memory:");
  printf("At $0200: %02X\n", memory[0x0200]);
  printf("At $0201: %02X\n", memory[0x0201]);
  printf("At $0202: %02X\n", memory[0x0202]);

  return 0;
}
