#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *regs[] = {"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
                      "t0",   "t1", "t2", "t3", "t4", "t5", "t6", "t7",
                      "t8",   "t9", "s0", "s1", "s2", "s3", "s4", "s5",
                      "s6",   "s7", "k0", "k1", "gp", "sp", "s8", "ra"};

void print_inst_r(uint32_t inst) {
  uint8_t rs, rt, rd, shift, funct;

  rs = (inst & 0x3E00000) >> 21;
  rt = (inst & 0x1F0000) >> 16;
  rd = (inst & 0xF800) >> 11;
  shift = (inst & 0x7C0) >> 6;
  funct = inst & 0x3F;

  switch (funct) {
  case 0x20:
    printf("add ");
    break;
  case 0x21:
    printf("addu ");
    break;
  case 0x24:
    printf("and ");
    break;
  case 0x1A:
    printf("div ");
    break;
  case 0x1B:
    printf("divu ");
    break;
  case 0x09:
    printf("jalr ");
    break;
  case 0x08:
    printf("jr ");
    break;
  case 0x10:
    printf("mfhi ");
    break;
  case 0x11:
    printf("mthi ");
    break;
  case 0x12:
    printf("mflo ");
    break;
  case 0x13:
    printf("mtlo ");
    break;
  case 0x18:
    printf("mult ");
    break;
  case 0x19:
    printf("multu ");
    break;
  case 0x27:
    printf("nor ");
    break;
  case 0x26:
    printf("xor ");
    break;
  case 0x25:
    printf("or ");
    break;
  case 0x2A:
    printf("slt ");
    break;
  case 0x2B:
    printf("sltu ");
    break;
  case 0x00:
    printf("sll $%s, $%s, %u\n", regs[rd], regs[rs], shift);
    return;
  case 0x02:
    printf("srl $%s, $%s, %u\n", regs[rd], regs[rs], shift);
    return;
  case 0x03:
    printf("sra $%s, $%s, %u\n", regs[rd], regs[rd], shift);
    return;
  case 0x22:
    printf("sub ");
    break;
  case 0x23:
    printf("subu ");
    break;
  default:
    printf("invalid instruction 0x%x\n", inst);
    return;
  }

  printf("$%s, $%s, $%s\n", regs[rd], regs[rs], regs[rt]);
}

void print_inst_j(uint8_t op, uint32_t inst, uint32_t pc) {
  uint32_t addr;

  addr = (inst & 0x3FFFFFF) << 2;
  addr |= pc & 0xF0000000;

  switch (op) {
  case 0x02:
    printf("j ");
    break;
  case 0x03:
    printf("jal ");
    break;
  }

  printf("0x%x\n", addr);
}

void print_inst_i(uint8_t op, uint32_t inst) {
  uint8_t rs, rt;
  uint16_t imm;

  rs = (inst & 0x3E00000) >> 21;
  rt = (inst & 0x1F0000) >> 16;
  imm = inst & 0xFFFF;

  switch (op) {
  case 0x08:
    printf("addi $%s, $%s, %d\n", regs[rt], regs[rs], imm);
    return;
  case 0x09:
    printf("addiu ");
    break;
  case 0x0C:
    printf("andi ");
    break;
  case 0x04:
    printf("beq ");
    break;
  case 0x06:
    printf("blez ");
    break;
  case 0x05:
    printf("bne ");
    break;
  case 0x07:
    printf("bgtz ");
    break;
  case 0x20:
    printf("lb ");
    break;
  case 0x24:
    printf("lbu ");
    break;
  case 0x25:
    printf("lhu ");
    break;
  case 0x0F:
    printf("lui ");
    break;
  case 0x23:
    printf("lw ");
    break;
  case 0x0D:
    printf("ori ");
    break;
  case 0x28:
    printf("sb ");
    break;
  case 0x29:
    printf("sh ");
    break;
  case 0x0A:
    printf("slti $%s, $%s, %d\n", regs[rt], regs[rs], imm);
    return;
  case 0x0B:
    printf("sltiu ");
    break;
  case 0x2B:
    printf("sw ");
    break;
  default:
    printf("invalid instruction 0x%x\n", inst);
    return;
  }

  printf("$%s, $%s, 0x%x\n", regs[rt], regs[rs], imm);
}

void print_inst(uint32_t inst, uint32_t pc) {
  uint8_t op;

  op = (inst & 0xFC000000) >> 26;
  if (op == 0x00) {
    print_inst_r(inst);
  } else if (op == 0x02 || op == 0x03) {
    print_inst_j(op, inst, pc);
  } else {
    print_inst_i(op, inst);
  }
}

int main(int argc, char **argv) {
  FILE *file;
  uint32_t inst, pc;

  if (argc != 2 && argc != 3) {
    fprintf(stderr, "usage: %s file.bin [start_addr]\n", argv[0]);
    return 1;
  }

  pc = 4;
  if (argc == 3) {
    if (strlen(argv[2]) < 3 || memcmp(argv[2], "0x", 2) != 0) {
      fprintf(stderr, "start_addr must be a hexadecimal value\n");
      return 1;
    }

    pc = strtol(argv[2] + 2, NULL, 16);
    if (pc == 0 && strcmp(argv[2], "0x0") != 0) {
      fprintf(stderr, "invalid value for start_addr\n");
      return 1;
    }
    pc += 4;
  }

  file = fopen(argv[1], "r");
  if (file == NULL) {
    perror("fopen()");
    return 1;
  }

  while (fread(&inst, sizeof(uint32_t), 1, file) == 1) {
    print_inst(inst, pc);
    pc += 4;
  }
  if (ferror(file)) {
    perror("fread()");
    fclose(file);
    return 1;
  }

  fclose(file);
  return 0;
}
