#include "decoder.h"

enum opcode decodeF(int instruction) {
  switch (instruction & 0xF0FF) {
    case 0xF007: 
      return LDDT;
    case 0xF00A:
      return LDK;
    case 0xF015: 
      return SDDT;
    case 0xF018: 
      return SDST;
    case 0xF01E:
      return ADDIR;
    case 0xF029: 
      return LDSPR;
    case 0xF033:
      return SDDEC;
    case 0xF055: 
      return SDR;
    case 0xF065:
      return LDR;
    default:
      return FAILED;
  }
}

enum opcode decodeE(int instruction) {
  switch (instruction & 0xF0FF) {
    case 0xE09E: // skip if pressed
      return SKK;
    case 0xE0A1: // skip if not pressed
      return SKNP;
    default:
      return FAILED;
  }
}

enum opcode decodeD(int instruction) {
  return DRW;
}

enum opcode decodeC(int instruction) {
  return RND;
}

enum opcode decodeB(int instruction) {
  return JPR;
}

enum opcode decodeA(int instruction) {
  return LDIR;
}

enum opcode decode9(int instruction) {
  if ((instruction & 0xF00F) == 0x9000) {
    return SKNE;
  } else {
    return FAILED;
  }
}

enum opcode decode8(int instruction) {
  switch (instruction & 0x000F) {
    case 0x000E:
      return SHL;
    case 0x0007:
      return SUBN;
    case 0x0006:
      return SHR;
    case 0x0005:
      return SUB;
    case 0x0004:
      return ADD;
    case 0x0003:
      return XOR;
    case 0x0002:
      return AND;
    case 0x0001:
      return OR;
    case 0x0000:
      return LD;
    default:
      return FAILED;
  }
}

enum opcode decode7(int instruction) {
  return ADDI;
}

enum opcode decode6(int instruction) {
  return LDI;
}

enum opcode decode5(int instruction) {
  return SE;
}

enum opcode decode4(int instruction) {
  return SKNEI;
}

enum opcode decode3(int instruction) {
  return SKEI;
}

enum opcode decode2(int instruction) {
  return CALL;
}

enum opcode decode1(int instruction) {
  return JP;
}

enum opcode decode0(int instruction) {
  switch(instruction) {
    case 0x00E0:
      return CLS;
    case 0x00EE:
      return RET;
    default:
      return SYS;
  }
}

enum opcode (*decodeOpcode[])(int) = { decode0, decode1, decode2, decode3, decode4, decode5, decode6, decode7, decode8, decode9, decodeA, decodeB, decodeC, decodeD, decodeE, decodeF };

enum opcode decode(int instruction) {
  int opcode = instruction >> 12;
  return decodeOpcode[opcode](instruction);
}
