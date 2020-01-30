#include <stdio.h>
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

unsigned int byte1 = 0;
unsigned int byte2 = 0;

unsigned int nibble1 = 0;
unsigned int nibble2 = 0;
unsigned int nibble3 = 0;
unsigned int nibble4 = 0;

int main(int argc, char* argv[]) {
  parse_opt(argc, argv);
  
  // read 1 byte into byte1 and byte2 each
  while(fscanf( stdin, "%1c%1c", (char*) &byte1, (char*) &byte2) == 2) {

    color(bold_blue);
    printf("0x%0X:\t", current_addr);

    // extract nibbles using bit masks
    nibble1 = (byte1 & 0xF0) >> 4;
    nibble2 = byte1 & 0xF;
    nibble3 = (byte2 & 0xF0) >> 4;
    nibble4 = byte2 & 0xF;

    color(bold_yellow);
    printf("0x%0X%0X%0X%0X\n", nibble1, nibble2, nibble3, nibble4);

    current_addr += 2;
  }
}

char* disassemble() {
  return NULL;
}
