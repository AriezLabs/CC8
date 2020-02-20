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

const char* version = "0.8";
const char* nickname = "visibly garbage";

int DEBUG = 0;

int SUCCESS = 1;
int FAILED = 0;

FILE* in;

short mem[MEMSIZE] = {};
uint64_t drawbuf[SCREENHEIGHT]; // each int is a full row of pixels
int drawbuf_changed = 0;

int clockspd = 500;

void draw() {
  // each line is actually two rows of pixels represented by these chars
  char* symbols[] = { [0] = " ", 
                      [1] = "▀", 
                      [2] = "▄",
                      [3] = "█"}; 

  // clear screen escape sequence. this is kind of slow - shit tier fps
  // maybe only redraw if buffer changed?
  if(!DEBUG)
    printf("\e[1;1H\e[2J");

  int i;
  int l;
  char line[192]; // █ etc. are non-ASCII chars (3 bytes long), hence 64*3 bytes

  for (l = 0; l < 32; l += 2) {
    memset(line, 0, sizeof(line));
    for (i = 63; i >= 0; i--) {
      int upper = ((drawbuf[l] >> i) & 1); // 1 if upper pixel active
      int lower = ((drawbuf[l + 1] >> (i - 1)) & 2); // 2 if lower pixel active
      int symbol = upper + lower; // => 0 if none, 1 if upper, 2 if lower, 3 if both
      strcat(line, symbols[symbol]); // the symbols arrays indices match this
    }
    puts(line);
  }
}

int run() {
  while(cycle()) {
    if (drawbuf_changed) {
      drawbuf_changed = 0;
      draw();
    }
    if (clockspd != 0)
      usleep(1000000 / clockspd);
  }
  return 1;
}

int get_instruction(int addr) {
  int instruction = mem[addr] << 8;
  return instruction + mem[addr + 1];
}

// copy rom into mem
void init_mem() {
  int current_addr = INITIAL_PC;
  // read rom bytewise to maintain big endianness
  while (fscanf(in, "%1c", (char*) (mem + current_addr)) == 1 && current_addr < MEMSIZE) 
    current_addr++;
  binary_len = current_addr;

  // load digits TODO join lines
  mem[0x0]=0xF0;
  mem[0x1]=0x90;
  mem[0x2]=0x90;
  mem[0x3]=0x90;
  mem[0x4]=0xF0;
  mem[0x5]=0x20;
  mem[0x6]=0x60;
  mem[0x7]=0x20;
  mem[0x8]=0x20;
  mem[0x9]=0x70;
  mem[0xa]=0xF0;
  mem[0xb]=0x10;
  mem[0xc]=0xF0;
  mem[0xd]=0x80;
  mem[0xe]=0xF0;
  mem[0xf]=0xF0;
  mem[0x10]=0x10;
  mem[0x11]=0xF0;
  mem[0x12]=0x10;
  mem[0x13]=0xF0;
  mem[0x14]=0x90;
  mem[0x15]=0x90;
  mem[0x16]=0xF0;
  mem[0x17]=0x10;
  mem[0x18]=0x10;
  mem[0x19]=0xF0;
  mem[0x1a]=0x80;
  mem[0x1b]=0xF0;
  mem[0x1c]=0x10;
  mem[0x1d]=0xF0;
  mem[0x1e]=0xF0;
  mem[0x1f]=0x80;
  mem[0x20]=0xF0;
  mem[0x21]=0x90;
  mem[0x22]=0xF0;
  mem[0x23]=0xF0;
  mem[0x24]=0x10;
  mem[0x25]=0x20;
  mem[0x26]=0x40;
  mem[0x27]=0x40;
  mem[0x28]=0xF0;
  mem[0x29]=0x90;
  mem[0x2a]=0xF0;
  mem[0x2b]=0x90;
  mem[0x2c]=0xF0;
  mem[0x2d]=0xF0;
  mem[0x2e]=0x90;
  mem[0x2f]=0xF0;
  mem[0x30]=0x10;
  mem[0x31]=0xF0;
  mem[0x32]=0xF0;
  mem[0x33]=0x90;
  mem[0x34]=0xF0;
  mem[0x35]=0x90;
  mem[0x36]=0x90;
  mem[0x37]=0xE0;
  mem[0x38]=0x90;
  mem[0x39]=0xE0;
  mem[0x3a]=0x90;
  mem[0x3b]=0xE0;
  mem[0x3c]=0xF0;
  mem[0x3d]=0x80;
  mem[0x3e]=0x80;
  mem[0x3f]=0x80;
  mem[0x40]=0xF0;
  mem[0x41]=0xE0;
  mem[0x42]=0x90;
  mem[0x43]=0x90;
  mem[0x44]=0x90;
  mem[0x45]=0xE0;
  mem[0x46]=0xF0;
  mem[0x47]=0x80;
  mem[0x48]=0xF0;
  mem[0x49]=0x80;
  mem[0x4a]=0xF0;
  mem[0x4b]=0xF0;
  mem[0x4c]=0x80;
  mem[0x4d]=0xF0;
  mem[0x4e]=0x80;
  mem[0x4f]=0x80;
}

void beep() {

}

void print_version() {
  printf("CC8 chip8 interpreter v%s '%s'\n", version, nickname);
}

int usage() {
  print_version();
  puts("usage:");
  puts("\t./CC8 { -r | -R | -d | -c | -h | -v | -s 0..9999 } [ path ]");
  puts("options:");
  puts("\t-r\trun");
  puts("\t-R\trun and print debug output");
  puts("\t-d\tdisassemble");
  puts("\t-c\tcolor output");
  puts("\t-h\tprint this help string");
  puts("\t-v\tprint version and exit");
  puts("\t-s\tset clock speed in Hz (default 500, 0 for uncapped speed)");
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
        if (clockspd < 0) {
          clockspd = 0;
          fprintf(stderr, "warning: setting clock speed to 0\n");
        }
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
    init_mem();
  
  return exec[mode]();
}
