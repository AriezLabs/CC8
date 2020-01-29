#include <stdio.h>
#include <string.h>
#include <getopt.h>

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

int main(int argc, char* argv[]) {
  parse_opt(argc, argv);

  unsigned int word1 = 0;
  unsigned int word2 = 0;
  unsigned int addr = 0;
  
  // read 1 byte into word1 and word2
  while(fscanf( stdin, "%1c%1c", (char*) &word1, (char*) &word2) == 2) {


    color(bold_blue);
    printf("0x%0X\t", addr);

    color(bold_yellow);
    printf("0x%02X%02X \n", word1, word2);

    addr += 2;
  }
}

char* disassemble() {
  return NULL;
}
