#ifndef CC8_h
#define CC8_h

int get_instruction(int addr);

int SUCCESS;
int FAILED;

int INITIAL_PC;

static enum opcode {
  SYS,      // 0nnn call routine
  CLS,      // 00E0 clear screen
  RET,      // 00EE set PC to *SP; SP--
  JP,       // 1nnn jump to nnn
  CALL,     // 2nnn push PC to stack, set PC to nnn
  SKEI,     // 3xkk skip next instruction if Vx == kk
  SKNEI,    // 4xkk skip next instruction if Vx != kk
  SE,       // 5xy0 skip next instruction if Vx == Vy
  LDI,      // 6xkk Vx = kk
  ADDI,     // 7xkk Vx += kk
  LD,       // 8xy0 Vx = Vy
  OR,       // 8xy1 Vx = Vx | Vy
  AND,      // 8xy2 Vx = Vx & Vy
  XOR,      // 8xy3 Vx = Vx XOR Vy
  ADD,      // 8xy4 Vx += Vy, VF = carry
  SUB,      // 8xy5 Vx = Vx - Vy, VF = NOT borrow (Vx > Vy)
  SHR,      // 8xy6 Vx = Vx >> 1 (ignore Vy)
  SUBN,     // 8xy7 Vx = Vy - Vx, VF = NOT borrow (Vy > Vx)
  SHL,      // 8xyE Vx = Vx << 1 (ignore Vy)
  SKNE,     // 9xy0 skip next instruction if Vx != Vy
  LDIR,     // Annn set register I = nnn
  JPR,      // Bnnn jump to V0 + nnn
  RND,      // Cxkk set Vx = random byte & kk
  DRW,      // Dxyn xor sprite at I to (Vx, Vy), wraparound, set VF = 1 iff pixel erased
  SKK,      // Ex9E skip next instruction if key Vx is pressed
  SKNP,     // ExA1 skip next instruction if key Vx is NOT pressed
  LDDT,     // Fx07 Vx = DT
  LDK,      // Fx0A wait for key press and store key id into Vx
  SDDT,     // Fx15 DT = Vx
  SDST,     // Fx18 ST = Vx
  ADDIR,    // Fx1E I += Vx
  LDSPR,    // Fx29 I = address of sprite for digit Vx
  SDDEC,    // Fx33 mem[i:i+2] = big endian 3-digit decimal representation of Vx
  SDR,      // Fx55 store registers V0-Vx at mem[i:...]
  LDR,      // Fx65 load mem[i:...] into V0-Vx
} opcode;

#endif
