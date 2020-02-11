#include <stdlib.h>

#include "decoder.h"
#include "CC8.h"

void decodeF(int instruction, decoded_instruction* i) {
  // the register might be source or destination depending on opcode, so just set both
  i->source = (instruction & 0xF00) >> 8;
  i->destination = (instruction & 0xF00) >> 8;

  switch (instruction & 0xF0FF) {
    case 0xF007: 
      i->op = LDDT;
      break;
    case 0xF00A:
      i->op = LDK;
      break;
    case 0xF015: 
      i->op = SDDT;
      break;
    case 0xF018: 
      i->op = SDST;
      break;
    case 0xF01E:
      i->op = ADDIR;
      break;
    case 0xF029: 
      i->op = LDSPR;
      break;
    case 0xF033:
      i->op = SDDEC;
      break;
    case 0xF055: 
      i->op = SDR;
      break;
    case 0xF065:
      i->op = LDR;
      break;
    default:
      i->op = UNKNOWN;
  }
}

void decodeE(int instruction, decoded_instruction* i) {
  i->source = (instruction & 0xF00) >> 8;

  switch (instruction & 0xF0FF) {
    case 0xE09E: // skip if pressed
      i->op = SKP;
      break;
    case 0xE0A1: // skip if not pressed
      i->op = SKNP;
      break;
    default:
      i->op = UNKNOWN;
  }
}

void decodeD(int instruction, decoded_instruction* i) {
  i->op = DRW;
  i->source = (instruction & 0xF00) >> 8;
  i->destination = (instruction & 0xF0) >> 4;
  i->immediate = (instruction & 0xF);
}

void decodeC(int instruction, decoded_instruction* i) {
  i->op = RND;
  i->destination = (instruction & 0xF00) >> 8;
  i->immediate = instruction & 0xFFF;
}

void decodeB(int instruction, decoded_instruction* i) {
  i->op = JPR;
  i->immediate = instruction & 0xFFF;
}

void decodeA(int instruction, decoded_instruction* i) {
  i->op = LDIR;
  i->immediate = instruction & 0xFFF;
}

void decode9(int instruction, decoded_instruction* i) {
  if ((instruction & 0xF) != 0x9000) {
    i->op = UNKNOWN;
    return;
  } 

  i->source = (instruction & 0xF00) >> 8;
  i->destination = (instruction & 0xF0) >> 4;
  i->op = SKNE;
}

void decode8(int instruction, decoded_instruction* i) {
  i->source = (instruction & 0xF00) >> 8;
  i->destination = (instruction & 0xF0) >> 4;

  switch (instruction & 0x000F) {
    case 0x000E:
      i->op = SHL;
      break;
    case 0x0007:
      i->op = SUBN;
      break;
    case 0x0006:
      i->op = SHR;
      break;
    case 0x0005:
      i->op = SUB;
      break;
    case 0x0004:
      i->op = ADD;
      break;
    case 0x0003:
      i->op = XOR;
      break;
    case 0x0002:
      i->op = AND;
      break;
    case 0x0001:
      i->op = OR;
      break;
    case 0x0000:
      i->op = LD;
      break;
    default:
      i->op = UNKNOWN;
  }
}

void decode7(int instruction, decoded_instruction* i) {
  i->op = ADDI;
  i->source = (instruction & 0xF00) >> 8;
  i->immediate = instruction & 0xFF;
}

void decode6(int instruction, decoded_instruction* i) {
  i->op = LDI;
  i->source = (instruction & 0xF00) >> 8;
  i->immediate = instruction & 0xFF;
}

void decode5(int instruction, decoded_instruction* i) {
  if ((instruction & 0xF) != 0) {
    i->op = UNKNOWN;
    return;
  }

  i->op = SE;
  i->source = (instruction & 0xF00) >> 8;
  i->destination = (instruction & 0xF0) >> 4;
}

void decode4(int instruction, decoded_instruction* i) {
  i->op = SKNEI;
  i->source = (instruction & 0xF00) >> 8;
  i->immediate = instruction & 0xFF;
}

void decode3(int instruction, decoded_instruction* i) {
  i->op = SKEI;
  i->source = (instruction & 0xF00) >> 8;
  i->immediate = instruction & 0xFF;
}

void decode2(int instruction, decoded_instruction* i) {
  i->op = CALL;
  i->immediate = instruction & 0xFFF;
}

void decode1(int instruction, decoded_instruction* i) {
  i->op = JP;
  i->immediate = instruction & 0xFFF;
}

void decode0(int instruction, decoded_instruction* i) {
  switch(instruction) {
    case 0x00E0:
      i->op = CLS;
    case 0x00EE:
      i->op = RET;
    default:
      i->op = SYS;
      i->immediate = instruction & 0xFFF;
  }
}

void (*decodeOpcode[])(int, decoded_instruction*) = { decode0, decode1, decode2, decode3, decode4, decode5, decode6, decode7, decode8, decode9, decodeA, decodeB, decodeC, decodeD, decodeE, decodeF };

decoded_instruction* decode(int instruction) {
  int opcode = instruction >> 12;
  decoded_instruction* i = malloc(sizeof(decoded_instruction));

  i->op = UNSET;
  i->source = UNSET;
  i->destination = UNSET;
  i->immediate = UNSET;

  decodeOpcode[opcode](instruction, i);
  return i;
}

