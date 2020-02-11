#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdint.h>
#include <errno.h>

#include "colors.h"
#include "CC8.h"
#include "disassembler.h"
#include "decoder.h"

#define MEMSIZE 4096

const char* version = "0.3";
const char* nickname = "neatly separated garbage";

int SUCCESS = 0;
int FAILED  = -1;

int INITIAL_PC = 0x200;

FILE* in;

short mem[MEMSIZE] = {};

int get_instruction(int addr) {
  int instruction = mem[addr] << 8;
  return instruction + mem[addr + 1];
}

// copy rom into mem
void copy_rom() {
  int current_addr = INITIAL_PC;
  // read rom bytewise to maintain big endianness
  while (fscanf(in, "%1c", (char*) (mem + current_addr)) == 1) 
    current_addr++;
}

void print_version() {
  printf("CC8 chip8 interpreter v%s '%s'\n", version, nickname);
}

int usage() {
  print_version();
  puts("usage:");
  puts("\t./CC8 { -d | -c | -h | -v } [ path ]");
  puts("options:");
  puts("\t-d\tdisassemble");
  puts("\t-c\tcolor output");
  puts("\t-h\tprint this help string");
  puts("\t-v\tprint version and exit");
  puts("\tpath\tfile to read from (defaults to stdin)");
  return FAILED;
}

int (*exec[])() = { usage, disassembleMem };
typedef enum { DEFAULT, DISASSEMBLE } mode;

int main(int argc, char* argv[]) {
  mode mode = DEFAULT;

  // parse flags
  char ch;
  while ((ch = getopt(argc, argv, "hvdc")) != EOF) {
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
      return FAILED;
    }
  } else { // default to reading from stdin
    in = stdin;
  }

  if (mode != DEFAULT)
    copy_rom();
  
  return exec[mode]();
}
