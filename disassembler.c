#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdint.h>

////////////
// COLORS //
////////////

char* red = "[0;31m";
char* bold_red = "[1;31m";
char* green = "[0;32m"; 
char* bold_green = "[1;32m"; 
char* yellow = "[0;33m";
char* bold_yellow = "[1;33m"; 
char* blue = "[0;34m";
char* bold_blue = "[1;34m"; 
char* magenta = "[0;35m"; 
char* bold_magenta = "[1;35m"; 
char* cyan = "[0;36m";
char* bold_cyan = "[1;36m"; 
char* reset = "[0m" ; 

int color_on = 0;

int FAILED = 0;
int SUCCESS = 1;

void color(char* color) {
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
  
}

int decodeE() {
  
}

int decodeD() {
  
}

int decodeC() {
  
}

int decodeB() {
  
}

int decodeA() {

}

int decode9() {
  
}

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

  printf("%s V%X V%X\n", 
      s,
      (instruction & 0xF00) >> 8, 
      (instruction & 0xF0) >> 4);

  return SUCCESS;
}

// ADD
int decode7() {
  printf("ADD V%X %02X\n", 
      (instruction & 0xF00) >> 8, 
      instruction & 0xFF);

  return SUCCESS;
}

// LD 
int decode6() {
  printf("LD V%X %02X\n", 
      (instruction & 0xF00) >> 8, 
      instruction & 0xFF);

  return SUCCESS;
}

// SE if equal registers
int decode5() {
  if ((instruction & 0xF00F) == 0x5000) {
    printf("SE V%X V%X\n", 
        (instruction & 0xF00) >> 8, 
        (instruction & 0xF0) >> 4);

    return SUCCESS;
  } else {
    return FAILED;
  }
}

// SNE skip not equal
int decode4() {
  printf("SNE V%X %02X\n", 
      (instruction & 0xF00) >> 8, 
      instruction & 0xFF);

  return SUCCESS;
}

// SE skip if equal
int decode3() {
  printf("SE V%X %02X\n", 
      (instruction & 0xF00) >> 8, 
      instruction & 0xFF);

  return SUCCESS;
}

// CALL
int decode2() {
  printf("CALL 0x%03X\n", 
      instruction & 0xFFF);

  return SUCCESS;
}

// JUMP
int decode1() {
  printf("JP 0x%03X\n", 
      instruction & 0xFFF);

  return SUCCESS;
}

// SYS, CLS, RET 
int decode0() {
  switch(instruction) {
    case 0x00E0:
      printf("CLS\n");
      break;
    case 0x00EE:
      printf("RET\n");
      break;
    default:
      printf("SYS 0x%03X\n", 
          instruction & 0xFFF);
  }

  return SUCCESS;
}

int (*decoder[])() = { decode0, decode1, decode2, decode3, decode4, decode5, decode6, decode7, decode8, decode9, decodeA, decodeB, decodeC, decodeD, decodeE, decodeF };

int main(int argc, char* argv[]) {
  parse_opt(argc, argv);
  
  // read 1 byte into byte1 and byte2 each
  while(fscanf( stdin, "%1c%1c", (char*) &byte1, (char*) &byte2) == 2) {

    color(bold_blue);
    printf("0x%0X:\t", current_addr);

    instruction |= byte1 << 8;
    instruction |= byte2;

    color(bold_yellow);
    printf("0x%04X\t", instruction);

    if (!decoder[instruction >> 12]()) {
      color(bold_red);
      puts("UNKNOWN INSTRUCTION");
    }

    instruction = 0;
    current_addr += 2;
  }
}
