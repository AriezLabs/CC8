#ifndef CC8_h
#define CC8_h

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

int get_instruction(int addr);

int SUCCESS;
int FAILED;

int INITIAL_PC;

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

#endif
