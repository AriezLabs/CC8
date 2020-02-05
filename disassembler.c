#include "colors.h"
#include "stdio.h"
#include "CC8.h"
#include "disassembler.h"

char** color_address = &cyan;
char** color_opcode = &bold_magenta;
char** color_iname = &bold_blue;
char** color_constant = &yellow;
char** color_register = &bold_green;
char** color_unknown = &bold_red;

int disassembleF(int instruction) {
  char* i;
  char* r;

  switch (instruction & 0xF0FF) {
    case 0xF015: // LD reg into delay timer
      i = "LD";
      r = "DT";
      break;
    case 0xF018: // LD reg into sound timer
      i = "LD";
      r = "ST";
      break;
    case 0xF01E: // ADD I + reg
      i = "ADD";
      r = "I";
      break;
    case 0xF029: // LD ptr to sprite &Vx into I
      i = "LD";
      r = "F";
      break;
    case 0xF033: // LD decimal of VX into [I]
      i = "LD";
      r = "B";
      break;
    case 0xF055: // LD V0-Vx into [I]
      i = "LD";
      r = "[I]";
      break;
    case 0xF007: // LD delay timer into reg
      color(*color_iname);
      printf("LD\t");
      color(*color_register);
      printf("V%X\t", (instruction & 0xF00) >> 8);
      printf("DT\n");
      return SUCCESS;
    case 0xF00A: // LD key press id
      color(*color_iname);
      printf("LD\t");
      color(*color_register);
      printf("V%X\t", (instruction & 0xF00) >> 8);
      printf("K\n");
      return SUCCESS;
    case 0xF065: //  LD mem starting at I into V0-Vx
      color(*color_iname);
      printf("LD\t");
      color(*color_register);
      printf("V%X\t", (instruction & 0xF00) >> 8);
      printf("[I]\n");
      return SUCCESS;
    default:
      return FAILED;
  }

  color(*color_iname);
  printf("%s\t", i);
  color(*color_register);
  printf("%s\t", r);
  printf("V%X\n", (instruction & 0xF00) >> 8);
  return SUCCESS;
}

// SKP SKNP
int disassembleE(int instruction) {
  char* s;

  switch (instruction & 0xF0FF) {
    case 0xE09E: // skip if pressed
      s = "SKP";
      break;
    case 0xE0A1: // skip if not pressed
      s = "SKNP";
      break;
    default:
      return FAILED;
  }

  color(*color_iname);
  printf("%s\t", s);
    color(*color_register);
  printf("V%X\n",
    (instruction & 0xF00) >> 8);
  return SUCCESS;
}

// DRW
int disassembleD(int instruction) {
  color(*color_iname);
  printf("DRW\t");
  color(*color_register);
  printf("V%X\t",
      (instruction & 0xF00) >> 8);
  printf("V%X\t", 
      (instruction & 0xF0) >> 4);
  color(*color_constant);
  printf("0x%X\n", instruction & 0x000F);
  return SUCCESS;
}

// RND
int disassembleC(int instruction) {
  color(*color_iname);
  printf("RND\t");
  color(*color_register);
  printf("V%X\t", (instruction & 0x0F00) >> 8);
  color(*color_constant);
  printf("0x%02X\n", instruction & 0x00FF);
  return SUCCESS;
}

// JP
int disassembleB(int instruction) {
  color(*color_iname);
  printf("JP\t");
  color(*color_register);
  printf("V0\t");
  color(*color_constant);
  printf("0x%03X\n", instruction & 0x0FFF);
  return SUCCESS;
}

// LD
int disassembleA(int instruction) {
  color(*color_iname);
  printf("LD\t");
  color(*color_register);
  printf("I\t");
  color(*color_constant);
  printf("0x%03X\n", instruction & 0x0FFF);
  return SUCCESS;
}

// SNE
int disassemble9(int instruction) {
  if ((instruction & 0xF00F) == 0x9000) {
    color(*color_iname);
    printf("SNE\t");
    color(*color_register);
    printf("V%X\t",
        (instruction & 0xF00) >> 8);
    printf("V%X\n", 
        (instruction & 0xF0) >> 4);

    return SUCCESS;
  } else {
    return FAILED;
  }
}

// SHL SUBN SHR SUB ADD XOR AND OR LD
int disassemble8(int instruction) {
  char* s;

  switch (instruction & 0x000F) {
    case 0x000E:
      s = "SHL";
      break;
    case 0x0007:
      s = "SUBN";
      break;
    case 0x0006:
      s = "SHR";
      break;
    case 0x0005:
      s = "SUB";
      break;
    case 0x0004:
      s = "ADD";
      break;
    case 0x0003:
      s = "XOR";
      break;
    case 0x0002:
      s = "AND";
      break;
    case 0x0001:
      s = "OR";
      break;
    case 0x0000:
      s = "LD";
      break;
    default:
      return FAILED;
  }

  color(*color_iname);
  printf("%s\t", s);
  color(*color_register);
  printf("V%X\t",
      (instruction & 0xF00) >> 8);
  printf("V%X\n", 
      (instruction & 0xF0) >> 4);

  return SUCCESS;
}

// ADD
int disassemble7(int instruction) {
  color(*color_iname);
  printf("ADD\t");
  color(*color_register);
  printf("V%X\t",
      (instruction & 0xF00) >> 8);
  color(*color_constant);
  printf("0x%02X\n", 
      instruction & 0xFF);

  return SUCCESS;
}

// LD 
int disassemble6(int instruction) {
  color(*color_iname);
  printf("LD\t");
  color(*color_register);
  printf("V%X\t",
      (instruction & 0xF00) >> 8);
  color(*color_constant);
  printf("0x%02X\n", 
      instruction & 0xFF);

  return SUCCESS;
}

// SE if equal egisters
int disassemble5(int instruction) {
  if ((instruction & 0xF00F) == 0x5000) {
    color(*color_iname);
    printf("SE\t");
    color(*color_register);
    printf("V%X\t",
        (instruction & 0xF00) >> 8);
    printf("V%X\n", 
        (instruction & 0xF0) >> 4);

    return SUCCESS;
  } else {
    return FAILED;
  }
}

// SNE skip not equal
int disassemble4(int instruction) {
  color(*color_iname);
  printf("SNE\t");
  color(*color_register);
  printf("V%X\t",
      (instruction & 0xF00) >> 8);
  color(*color_constant);
  printf("0x%02X\n", 
      instruction & 0xFF);

  return SUCCESS;
}

// SE skip if equal
int disassemble3(int instruction) {
  color(*color_iname);
  printf("SE\t");
  color(*color_register);
  printf("V%X\t",
      (instruction & 0xF00) >> 8);
  color(*color_constant);
  printf("0x%02X\n", 
      instruction & 0xFF);

  return SUCCESS;
}

// CALL
int disassemble2(int instruction) {
  color(*color_iname);
  printf("CALL\t");
  color(*color_constant);
  printf("0x%03X\n", 
      instruction & 0xFFF);

  return SUCCESS;
}

// JUMP
int disassemble1(int instruction) {
  color(*color_iname);
  printf("JP\t");
  color(*color_constant);
  printf("0x%03X\n", 
      instruction & 0xFFF);

  return SUCCESS;
}

// SYS, CLS, RET 
int disassemble0(int instruction) {
  color(*color_iname);
  switch(instruction) {
    case 0x00E0:
      printf("CLS\n");
      break;
    case 0x00EE:
      printf("RET\n");
      break;
    default:
      printf("SYS\t");
      color(*color_constant);
      printf("0x%03X\n", 
          instruction & 0xFFF);
  }

  return SUCCESS;
}

int (*disassembleOpcode[])(int) = { disassemble0, disassemble1, disassemble2, disassemble3, disassemble4, disassemble5, disassemble6, disassemble7, disassemble8, disassemble9, disassembleA, disassembleB, disassembleC, disassembleD, disassembleE, disassembleF };

int disassembleMem() {
  int current_addr = INITIAL_PC;
  int instruction;
  int opcode;

  while((instruction = get_instruction(current_addr))) {
    opcode = instruction >> 12;

    color(*color_address);
    printf("0x%0X:\t", current_addr);

    color(*color_opcode);
    printf("0x%04X\t", instruction);

    // TODO decouple decoding and disassembling/printing
    // such that disassembling vs emulating is just switching functors
    if (disassembleOpcode[opcode](instruction) == FAILED) {
      color(*color_unknown);
      puts("UNKNOWN");
    }

    current_addr += 2;
  }

  return SUCCESS;
}

