#include "colors.h"
#include "stdio.h"
#include "CC8.h"
#include "disassembler.h"
#include "decoder.h"
#include <stdlib.h>

char** color_address = &cyan;
char** color_opcode = &bold_magenta;
char** color_iname = &bold_blue;
char** color_constant = &red;
char** color_register = &bold_green;
char** color_unknown = &bold_yellow;

int disassembleMem() {
  int current_addr = INITIAL_PC;
  int instruction;
  int opcode;

  // TODO read to first actual zero
  while((instruction = get_instruction(current_addr))) {
    decoded_instruction* i = decode(instruction);

    color(*color_address);
    printf("0x%0X:\t", current_addr);

    color(*color_opcode);
    printf("0x%04X\t", instruction);

    if (i->op == UNKNOWN) {
      color(*color_unknown);
      printf("%s\t", opcode_literals[i->op]);

    } else {
      color(*color_iname);
      printf("%s\t", opcode_literals[i->op]);

      color(*color_register);
      if (i->source != UNSET)
        printf("V%X\t", i->source);
      if (i->destination != UNSET)
        printf("V%X\t", i->destination);

      color(*color_constant);
      if (i->immediate != UNSET)
        printf("0x%X\t", i->immediate);
    }

    puts("");
    current_addr += 2;
    free(i);
  }

  return SUCCESS;
}

