#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#include "colors.h"
#include "CC8.h"
#include "disassembler.h"
#include "decoder.h"
#include "cpu.h"

const char* version = "0.6";
const char* nickname = "garbage with a brain and a canvas";

int DEBUG   = 0;
int SUCCESS = 1;
int FAILED  = 0;

FILE* in;

short mem[MEMSIZE] = {};
uint64_t drawbuf[SCREENHEIGHT]; // each int is a full row of pixels

int clockspd = 500;

void draw() {
  // each line is actually two rows of pixels represented by these chars
  char* symbols[] = { [0] = " ", 
                      [1] = "▀", 
                      [2] = "▄",
                      [3] = "█"}; 

  // clear screen escape sequence
  if(!DEBUG)
    printf("\e[1;1H\e[2J");

  int64_t i;
  int64_t l;
  char line[200]; // █ etc. are non-ASCII chars (3 bytes long)

  for (l = 0; l < 32; l += 2) {
    memset(line, 0, sizeof(line));
    for (i = 63; i >= 0; i--) {
      int64_t upper = ((drawbuf[l] >> i) & 1); // 1 if upper pixel active
      int64_t lower = ((drawbuf[l + 1] >> (i - 1)) & 2); // 2 if lower pixel active
      int64_t symbol = upper + lower; // 0 if none, 1 if upper, 2 if lower, 3 if both
      strcat(line, symbols[symbol]);
    }
    puts(line);
  }
}

int run() {
  while(cycle()) {
    draw();
    usleep(1000000 / clockspd);
  }
  return 1;
}

int get_instruction(int addr) {
  int instruction = mem[addr] << 8;
  return instruction + mem[addr + 1];
}

// copy rom into mem
void copy_rom() {
  int current_addr = INITIAL_PC;
  // read rom bytewise to maintain big endianness
  while (fscanf(in, "%1c", (char*) (mem + current_addr)) == 1 && current_addr < MEMSIZE) 
    current_addr++;
  binary_len = current_addr;
}

void beep() {

}

void print_version() {
  printf("CC8 chip8 interpreter v%s '%s'\n", version, nickname);
}

int usage() {
  print_version();
  puts("usage:");
  puts("\t./CC8 { -r | -R | -d | -c | -h | -v | -s 1..9999 } [ path ]");
  puts("options:");
  puts("\t-r\trun");
  puts("\t-R\trun and print debug output");
  puts("\t-d\tdisassemble");
  puts("\t-c\tcolor output");
  puts("\t-h\tprint this help string");
  puts("\t-v\tprint version and exit");
  puts("\t-s\tset ~clock speed in Hz (default 500)");
  puts("\tpath\tfile to read from (defaults to stdin)");
  return 0;
}

int (*exec[])() = { usage, disassembleMem, run };
typedef enum { DEFAULT, DISASSEMBLE, RUN } mode;

int main(int argc, char* argv[]) {
  mode mode = DEFAULT;

  // parse flags
  char ch;
  while ((ch = getopt(argc, argv, "rRhvdcs:")) != EOF) {
    switch (ch) {
      case 'v':
        print_version();
        return SUCCESS;
      case 'c':
        color_on = 1;
        break;
      case 'd':
        mode = DISASSEMBLE;
        break;
      case 'R':
        mode = DEBUG = 1;
      case 'r':
        mode = RUN;
        break;
      case 's':
        clockspd = atoi(optarg);
        break;
    }
  }

  argc -= optind;
  argv += optind;

  // get filename
  if (argc >= 1) {
    if (argc > 1) 
      fprintf(stderr, "warning: ignoring any arguments after %s\n", *argv);

    in = fopen(*argv, "r");

    if (in == NULL) {
      perror(*argv);
      return 1;
    }
  } else { // default to reading from stdin
    in = stdin;
  }

  if (mode != DEFAULT)
    copy_rom();
  
  return exec[mode]();
}
