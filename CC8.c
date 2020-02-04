#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdint.h>
#include <errno.h>
#include "colors.h"

const char* version = "0.1";
const char* nickname = "useless garbage";

const int SUCCESS = 0;
const int FAILED  = 1;

FILE* in;

//////////////////
// DISASSEMBLER //
//////////////////

char** color_address = &cyan;
char** color_opcode = &bold_magenta;
char** color_iname = &bold_blue;
char** color_constant = &yellow;
char** color_register = &bold_green;
char** color_unknown = &bold_red;

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

int (*decode[])() = { decode0, decode1, decode2, decode3, decode4, decode5, decode6, decode7, decode8, decode9, decodeA, decodeB, decodeC, decodeD, decodeE, decodeF };

int disassemble() {
  // read 1 byte into byte1 and byte2 each
  while(fscanf(in, "%1c%1c", (char*) &byte1, (char*) &byte2) == 2) {

    instruction |= byte1 << 8;
    instruction |= byte2;

    color(*color_address);
    printf("0x%0X:\t", current_addr);

    color(*color_opcode);
    printf("0x%04X\t", instruction);

    if (decode[instruction >> 12]() == FAILED) {
      color(*color_unknown);
      puts("UNKNOWN");
    }

    instruction = 0;
    current_addr += 2;
  }

  return SUCCESS;
}

/////////////
// MAIN FN //
/////////////

int unset() {
  printf("CC8 chip8 interpreter v%s '%s'\n", version, nickname);
  puts("usage:");
  puts("\t./CC8 { -d | -c } [ path ]");
  puts("options:");
  puts("\t-d\tdisassemble");
  puts("\t-c\tcolor output");
  puts("\tpath\tfile to read from (defaults to stdin)");
  return SUCCESS; // do nothing successfully
}

int (*exec[])() = { unset, disassemble };
typedef enum { UNSET, DISASSEMBLE } mode;

int main(int argc, char* argv[]) {
  mode mode = UNSET;

  char ch;
  while ((ch = getopt(argc, argv, "dc")) != EOF) {
    switch (ch) {
      case 'c':
        color_on = 1;
        break;
      case 'd':
        mode = DISASSEMBLE;
        break;
    }
  }

  argc -= optind;
  argv += optind;

  if (argc >= 1) {
    if (argc > 1) 
      fprintf(stderr, "warning: ignoring any arguments after %s\n", *argv);

    in = fopen(*argv, "r");

    if (in == NULL) {
      perror("ERROR");
      return FAILED;
    }
  } else { // default to reading from stdin
    in = stdin;
  }
  
  return exec[mode]();
}
