#include "cpu.h"
#include <stdio.h>

// 0nnn call routine
void do_sys(decoded_instruction* instruction) {

}

// 00E0 clear screen
void do_cls(decoded_instruction* instruction) {

}

// 00EE set PC to *SP; SP--
void do_ret(decoded_instruction* instruction) {

}

// 1nnn jump to nnn
void do_jp(decoded_instruction* instruction) {

}

// 2nnn push PC to stack, set PC to nnn
void do_call(decoded_instruction* instruction) {

}

// 3xkk skip next instruction if Vx == kk
void do_skei(decoded_instruction* instruction) {

}

// 4xkk skip next instruction if Vx != kk
void do_sknei(decoded_instruction* instruction) {

}

// 5xy0 skip next instruction if Vx == Vy
void do_se(decoded_instruction* instruction) {

}

// 6xkk Vx = kk
void do_ldi(decoded_instruction* instruction) {

}

// 7xkk Vx += kk
void do_addi(decoded_instruction* instruction) {

}

// 8xy0 Vx = Vy
void do_ld(decoded_instruction* instruction) {

}

// 8xy1 Vx = Vx | Vy
void do_or(decoded_instruction* instruction) {

}

// 8xy2 Vx = Vx & Vy
void do_and(decoded_instruction* instruction) {

}

// 8xy3 Vx = Vx XOR Vy
void do_xor(decoded_instruction* instruction) {

}

// 8xy4 Vx += Vy, VF = carry
void do_add(decoded_instruction* instruction) {

}

// 8xy5 Vx = Vx - Vy, VF = NOT borrow (Vx > Vy)
void do_sub(decoded_instruction* instruction) {

}

// 8xy6 Vx = Vx >> 1 (ignore Vy)
void do_shr(decoded_instruction* instruction) {

}

// 8xy7 Vx = Vy - Vx, VF = NOT borrow (Vy > Vx)
void do_subn(decoded_instruction* instruction) {

}

// 8xyE Vx = Vx << 1 (ignore Vy)
void do_shl(decoded_instruction* instruction) {

}

// 9xy0 skip next instruction if Vx != Vy
void do_skne(decoded_instruction* instruction) {

}

// Annn set register I = nnn
void do_ldir(decoded_instruction* instruction) {

}

// Bnnn jump to V0 + nnn
void do_jpr(decoded_instruction* instruction) {

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

void do_instruction(decoded_instruction* instruction) {
  if (instruction->op == UNKNOWN) {
    puts("GET BETTER HANDLING FOR UNKNOWN INSTRUCTIONS RETARD");
    return;
  }

  opcodes[instruction->op](instruction);
}
