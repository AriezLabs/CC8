#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdint.h>

////////////
// COLORS //
////////////

const char* red = "[0;31m";
const char* bold_red = "[1;31m";
const char* green = "[0;32m"; 
const char* bold_green = "[1;32m"; 
const char* yellow = "[0;33m";
const char* bold_yellow = "[1;33m"; 
const char* blue = "[0;34m";
const char* bold_blue = "[1;34m"; 
const char* magenta = "[0;35m"; 
const char* bold_magenta = "[1;35m"; 
const char* cyan = "[0;36m";
const char* bold_cyan = "[1;36m"; 
const char* reset = "[0m" ; 

const int FAILED = 0;
const int SUCCESS = 1;

const char** color_address = &cyan;
const char** color_opcode = &bold_magenta;
const char** color_iname = &bold_blue;
const char** color_constant = &yellow;
const char** color_register = &bold_green;
const char** color_unknown = &bold_red;

int color_on = 0;

void color(const char* color) {
  if (color_on)
    printf("\033%s", color);
}

void parse_opt(int argc, char* argv[]) {
  char ch;
  while ((ch = getopt(argc, argv, "c")) != EOF) {
    switch (ch) {
      case 'c':
        color_on = 1;
        break;
    }
  }
  argc += optind;
  argv += optind;
}

//////////////////
// DISASSEMBLER //
//////////////////

unsigned int current_addr = 0x200; // start address is 0x200

unsigned short byte1 = 0;
unsigned short byte2 = 0;

unsigned short instruction = 0;

char* instruction_string;

int decodeF() {
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
int decodeE() {
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
int decodeD() {
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
int decodeC() {
  color(*color_iname);
  printf("RND\t");
  color(*color_register);
  printf("V%X\t", (instruction & 0x0F00) >> 8);
  color(*color_constant);
  printf("0x%02X\n", instruction & 0x00FF);
  return SUCCESS;
}

// JP
int decodeB() {
  color(*color_iname);
  printf("JP\t");
  color(*color_register);
  printf("V0\t");
  color(*color_constant);
  printf("0x%03X\n", instruction & 0x0FFF);
  return SUCCESS;
}

// LD
int decodeA() {
  color(*color_iname);
  printf("LD\t");
  color(*color_register);
  printf("I\t");
  color(*color_constant);
  printf("0x%03X\n", instruction & 0x0FFF);
  return SUCCESS;
}

// SNE
int decode9() {
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
int decode8() {
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
int decode7() {
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
int decode6() {
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
int decode5() {
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
int decode4() {
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
int decode3() {
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
int decode2() {
  color(*color_iname);
  printf("CALL\t");
  color(*color_constant);
  printf("0x%03X\n", 
      instruction & 0xFFF);

  return SUCCESS;
}

// JUMP
int decode1() {
  color(*color_iname);
  printf("JP\t");
  color(*color_constant);
  printf("0x%03X\n", 
      instruction & 0xFFF);

  return SUCCESS;
}

// SYS, CLS, RET 
int decode0() {
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

int (*decoder[])() = { decode0, decode1, decode2, decode3, decode4, decode5, decode6, decode7, decode8, decode9, decodeA, decodeB, decodeC, decodeD, decodeE, decodeF };

int main(int argc, char* argv[]) {
  parse_opt(argc, argv);
  
  // read 1 byte into byte1 and byte2 each
  while(fscanf( stdin, "%1c%1c", (char*) &byte1, (char*) &byte2) == 2) {

    instruction |= byte1 << 8;
    instruction |= byte2;

    color(*color_address);
    printf("0x%0X:\t", current_addr);

    color(*color_opcode);
    printf("0x%04X\t", instruction);

    if (decoder[instruction >> 12]() == FAILED) {
      color(*color_unknown);
      puts("UNKNOWN");
    }

    instruction = 0;
    current_addr += 2;
  }
}
