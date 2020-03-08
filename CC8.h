#ifndef CC8_h
#define CC8_h

#include <stdint.h>

// define opcodes as enum and string via preprocessor
#define FOREACH_OPCODE(OPCODE) \
  OPCODE(SYS)     \
  OPCODE(CLS)     \
  OPCODE(RET)     \
  OPCODE(JP)      \
  OPCODE(CALL)    \
  OPCODE(SKEI)    \
  OPCODE(SKNEI)   \
  OPCODE(SE)      \
  OPCODE(LDI)     \
  OPCODE(ADDI)    \
  OPCODE(LD)      \
  OPCODE(OR)      \
  OPCODE(AND)     \
  OPCODE(XOR)     \
  OPCODE(ADD)     \
  OPCODE(SUB)     \
  OPCODE(SHR)     \
  OPCODE(SUBN)    \
  OPCODE(SHL)     \
  OPCODE(SKNE)    \
  OPCODE(LDIR)    \
  OPCODE(JPR)     \
  OPCODE(RND)     \
  OPCODE(DRW)     \
  OPCODE(SKP)     \
  OPCODE(SKNP)    \
  OPCODE(LDDT)    \
  OPCODE(LDK)     \
  OPCODE(SDDT)    \
  OPCODE(SDST)    \
  OPCODE(ADDIR)   \
  OPCODE(LDSPR)   \
  OPCODE(SDDEC)   \
  OPCODE(SDR)     \
  OPCODE(LDR)     \
  OPCODE(UNKNOWN) 

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define MEMSIZE 4096
#define SCREENHEIGHT 32
#define INITIAL_PC 0x200

const static int UNMAPPED_KEY = 9001;
static int kbmap[256] = { 
  [0 ... 255] = UNMAPPED_KEY,
  ['x'] = 0x0,
  ['1'] = 0x1,
  ['2'] = 0x2,
  ['3'] = 0x3,
  ['q'] = 0x4,
  ['w'] = 0x5,
  ['e'] = 0x6,
  ['a'] = 0x7,
  ['s'] = 0x8,
  ['d'] = 0x9,
  ['z'] = 0xA,
  ['c'] = 0xB,
  ['4'] = 0xC,
  ['r'] = 0xD,
  ['f'] = 0xE,
  ['v'] = 0xF,
};

int binary_len;

int get_instruction(int addr);

int DEBUG;

int SUCCESS;
int FAILED;

// extern - defined once in a source file
extern short mem[];

uint64_t drawbuf[32];
int drawbuf_changed;

// static - defined this way once for every source file
// (mutations won't change var for other source files)
static const char* opcode_literals[] = {
  FOREACH_OPCODE(GENERATE_STRING)
};

static enum opcode {
  FOREACH_OPCODE(GENERATE_ENUM)
} opcode;

typedef struct {
  enum opcode op;
  int source;
  int destination;
  int immediate;
} decoded_instruction;

void beep();

#endif
