#define NUMREGS 16
#define STACKSIZE 1024

#include "cpu.h"
#include "colors.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int V[NUMREGS];            // GP registers
int I;                     // address register
int DT = 0;                // delay timer register
int ST = 0;                // sound timer register
int PC = INITIAL_PC;       // program counter
int SP = 0;                // stack pointer
int stack[STACKSIZE];      // stack

unsigned long long cycle_count = 0;

void debug_error(char* format_string, ...) {
  if (DEBUG) {
    va_list ap;
    va_start(ap, format_string);
    color(red);
    printf("[ERROR] ");
    vprintf(format_string, ap);
    color(reset);
  }
}

void debug_print(char* format_string, ...) {
  if (DEBUG) {
    va_list ap;
    va_start(ap, format_string);
    vprintf(format_string, ap);
  }
}

// 0nnn call routine
void do_sys(decoded_instruction* instruction) {
  debug_print("\tSYS: ignoring\n", instruction->immediate);
}

// 00E0 clear screen
void do_cls(decoded_instruction* instruction) {
  int i;
  for (i = 0; i < SCREENHEIGHT; i++)
    drawbuf[i] = 0LL;
  debug_print("\tCLS: cleared draw buffer\n");
}

// 00EE set PC to *SP; SP--
void do_ret(decoded_instruction* instruction) {
  if (SP > 0) {
    PC = stack[SP];
    SP--;

    debug_print("\tRET: PC=0x%04X, SP=0x%04X\n", PC, SP);
  }
  else
    debug_error("\tRET: SP is zero, ignoring instruction\n");
}

// 1nnn jump to nnn
void do_jp(decoded_instruction* instruction) {
  PC = instruction->immediate;

  debug_print("\tJP: PC=0x%04X\n", PC);
}

// 2nnn push PC to stack, set PC to nnn
void do_call(decoded_instruction* instruction) {
  SP++;
  stack[SP] = PC;
  PC = instruction->immediate;

  debug_print("\tCALL: PC=0x%04X, SP=0x%04X, stack[SP]=0x%04X\n", PC, SP, stack[SP]);
}

// 3xkk skip next instruction if Vx == kk
void do_skei(decoded_instruction* instruction) {
  if (V[instruction->source] == instruction->immediate)
    PC += 2;

  debug_print("\tSKEI: V%d=%d, imm=%d ⇒ PC=0x%04X\n", 
      instruction->source,
      V[instruction->source],
      instruction->immediate,
      PC);
}

// 4xkk skip next instruction if Vx != kk
void do_sknei(decoded_instruction* instruction) {
  if (V[instruction->source] != instruction->immediate)
    PC += 2;

  debug_print("\tSKNEI: V%d=%d, imm=%d ⇒ PC=0x%04X\n", 
      instruction->source,
      V[instruction->source],
      instruction->immediate,
      PC);
}

// 5xy0 skip next instruction if Vx == Vy
void do_se(decoded_instruction* instruction) {
  if (V[instruction->source] == V[instruction->destination])
    PC += 2;

  debug_print("\tSE: V%d=%d, V%d=%d ⇒ PC=0x%04X\n", 
      instruction->source,
      V[instruction->source],
      instruction->destination,
      V[instruction->destination],
      PC);
}

// 6xkk Vx = kk
void do_ldi(decoded_instruction* instruction) {
  V[instruction->destination] = instruction->immediate;

  debug_print("\tLDI: imm=%d ⇒ V%d=%d\n", 
      instruction->immediate,
      instruction->destination,
      V[instruction->destination]);
}

// 7xkk Vx += kk
void do_addi(decoded_instruction* instruction) {
  V[instruction->destination] += instruction->immediate;

  debug_print("\tADDI: imm=%d ⇒ V%d=%d\n", 
      instruction->immediate,
      instruction->destination,
      V[instruction->destination]);
}

// 8xy0 Vx = Vy
void do_ld(decoded_instruction* instruction) {
  V[instruction->destination] = V[instruction->source];

  debug_print("\tLD: V%d=%d ⇒ V%d=%d\n", 
      instruction->source,
      V[instruction->source],
      instruction->destination,
      V[instruction->destination]);
}

// 8xy1 Vx = Vx | Vy
void do_or(decoded_instruction* instruction) {
  debug_print("\tOR: V%d=%d | V%d=%d", 
      instruction->source,
      V[instruction->source],
      instruction->destination,
      V[instruction->destination]);

  V[instruction->destination] |= V[instruction->source];

  debug_print(" ⇒ V%d=%d\n", 
      instruction->destination,
      V[instruction->destination]);
}

// 8xy2 Vx = Vx & Vy
void do_and(decoded_instruction* instruction) {
  debug_print("\tAND: V%d=%d & V%d=%d", 
      instruction->source,
      V[instruction->source],
      instruction->destination,
      V[instruction->destination]);

  V[instruction->destination] &= V[instruction->source];

  debug_print(" ⇒ V%d=%d\n", 
      instruction->destination,
      V[instruction->destination]);
}

// 8xy3 Vx = Vx XOR Vy
void do_xor(decoded_instruction* instruction) {
  debug_print("\tXOR: V%d=%d ^ V%d=%d", 
      instruction->source,
      V[instruction->source],
      instruction->destination,
      V[instruction->destination]);

  V[instruction->destination] ^= V[instruction->source];

  debug_print(" ⇒ V%d=%d\n", 
      instruction->destination,
      V[instruction->destination]);
}

// 8xy4 Vx += Vy, VF = carry
void do_add(decoded_instruction* instruction) {
  debug_print("\tADD: V%d=%d + V%d=%d", 
      instruction->source,
      V[instruction->source],
      instruction->destination,
      V[instruction->destination]);

  V[instruction->destination] += V[instruction->source];

  if (V[instruction->destination] > 0xFF) {
    V[instruction->destination] &= 0xFF;
    V[0xF] = 1;
  }

  debug_print(" ⇒ V%d=%d, VF=%d\n", 
      instruction->destination,
      V[instruction->destination],
      V[0xF]);
}

// 8xy5 Vx = Vx - Vy, VF = NOT borrow (Vx > Vy)
void do_sub(decoded_instruction* instruction) {
  debug_print("\tSUB: V%d=%d - V%d=%d", 
      instruction->source,
      V[instruction->source],
      instruction->destination,
      V[instruction->destination]);

  V[instruction->destination] -= V[instruction->source];

  if (V[instruction->destination] > V[instruction->source]) {
    V[0xF] = 1;
  } else {
    V[0xF] = 0;
    V[instruction->destination] &= 0xFF;
  }

  debug_print(" ⇒ V%d=%d, VF=%d\n", 
      instruction->destination,
      V[instruction->destination],
      V[0xF]);
}

// 8xy6 Vx = Vx >> 1 (ignore Vy)
void do_shr(decoded_instruction* instruction) {
  debug_print("\tSHR: V%d=%d", 
      instruction->destination,
      V[instruction->destination]);

  V[instruction->destination] >>= 1;

  debug_print(" ⇒ V%d=%d\n", 
      instruction->destination,
      V[instruction->destination]);
}

// 8xy7 Vx = Vy - Vx, VF = NOT borrow (Vy > Vx)
void do_subn(decoded_instruction* instruction) {
  debug_print("\tSUBN: V%d=%d - V%d=%d", 
      instruction->source,
      V[instruction->source],
      instruction->destination,
      V[instruction->destination]);

  V[instruction->destination] = V[instruction->source] - V[instruction->destination];

  if (V[instruction->source] > V[instruction->destination]) {
    V[0xF] = 1;
  } else {
    V[0xF] = 0;
    V[instruction->destination] &= 0xFF;
  }

  debug_print(" ⇒ V%d=%d, VF=%d\n", 
      instruction->destination,
      V[instruction->destination],
      V[0xF]);
}

// 8xyE Vx = Vx << 1 (ignore Vy)
void do_shl(decoded_instruction* instruction) {
  debug_print("\tSHL: V%d=%d", 
      instruction->destination,
      V[instruction->destination]);

  V[instruction->destination] >>= 1;
  V[instruction->destination] &= 0xFF;

  debug_print(" ⇒ V%d=%d\n", 
      instruction->destination,
      V[instruction->destination]);
}

// 9xy0 skip next instruction if Vx != Vy
void do_skne(decoded_instruction* instruction) {
  if (V[instruction->source] != V[instruction->destination])
    PC += 2;

  debug_print("\tSKNE: V%d=%d, V%d=%d ⇒ PC=0x%04X\n", 
      instruction->source,
      V[instruction->source],
      instruction->destination,
      V[instruction->destination],
      PC);
}

// Annn set register I = nnn
void do_ldir(decoded_instruction* instruction) {
  debug_print("\tLDIR: I=0x%04X", I);

  I = instruction->immediate;

  debug_print(" ⇒ I=0x%04X\n", I);
}

// Bnnn jump to V0 + nnn
void do_jpr(decoded_instruction* instruction) {
  debug_print("\tJPR: PC=0x%04X", PC);

  PC = instruction->immediate + V[0];

  debug_print(" ⇒ PC=0x%04X\n", PC);
}

// Cxkk set Vx = random byte & kk
void do_rnd(decoded_instruction* instruction) {
}

// Dxyn xor n-byte sprite at I to (Vx, Vy), wraparound, set VF = 1 iff pixel erased
void do_drw(decoded_instruction* instruction) {
}

// Ex9E skip next instruction if key Vx is pressed
void do_skp(decoded_instruction* instruction) {
}

// ExA1 skip next instruction if key Vx is NOT pressed
void do_sknp(decoded_instruction* instruction) {
}

// Fx07 Vx = DT
void do_lddt(decoded_instruction* instruction) {
}

// Fx0A wait for key press and store key id into Vx
void do_ldk(decoded_instruction* instruction) {
}

// Fx15 DT = Vx
void do_sddt(decoded_instruction* instruction) {
}

// Fx18 ST = Vx
void do_sdst(decoded_instruction* instruction) {
}

// Fx1E I += Vx
void do_addir(decoded_instruction* instruction) {
}

// Fx29 I = address of sprite for digit Vx
void do_ldspr(decoded_instruction* instruction) {
}

// Fx33 mem[i:i+2] = big endian 3-digit decimal representation of Vx
void do_sddec(decoded_instruction* instruction) {
}

// Fx55 store registers V0-Vx at mem[i:...]
void do_sdr(decoded_instruction* instruction) {
}

// Fx65 load mem[i:...] into V0-Vx
void do_ldr(decoded_instruction* instruction) {
}

void (*opcodes[])(decoded_instruction*) = { do_sys, do_cls, do_ret, do_jp, do_call, do_skei, do_sknei, do_se, do_ldi, do_addi, do_ld, do_or, do_and, do_xor, do_add, do_sub, do_shr, do_subn, do_shl, do_skne, do_ldir, do_jpr, do_rnd, do_drw, do_skp, do_sknp, do_lddt, do_ldk, do_sddt, do_sdst, do_addir, do_ldspr, do_sddec, do_sdr, do_ldr };

int do_instruction(decoded_instruction* instruction) {
  if (instruction->op == UNKNOWN) {
    debug_error("ran into unknown instruction\n");
    return FAILED;
  }

  opcodes[instruction->op](instruction);
  return SUCCESS;
}

int cycle() {
  debug_print("cycle %lld (PC=0x%04X):\t", cycle_count, PC);

  if (DT > 0) {
    debug_print("DT: %d -> %d\t", DT, DT - 1);
    DT--;
  }

  if (ST > 0) {
    debug_print("ST: %d -> %d\t", ST, ST - 1);
    ST--;
    beep();
  }

  decoded_instruction* i = decode(get_instruction(PC));

  int status = do_instruction(i);
  PC += 2;
  cycle_count++;
  free(i);
  return status;
}
