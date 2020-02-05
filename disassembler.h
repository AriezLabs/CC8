#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

static const char* opcodeLiterals[] = {
  "SYS",
  "CLS",
  "RET",
  "JP",
  "CALL",
  "SKEI",
  "SKNEI",
  "SE",
  "LDI",
  "ADDI",
  "LD",
  "OR",
  "AND",
  "XOR",
  "ADD",
  "SUB",
  "SHR",
  "SUBN",
  "SHL",
  "SKNE",
  "LDIR",
  "JPR",
  "RND",
  "DRW",
  "SKK",
  "SKNP",
  "LDDT",
  "LDK",
  "SDDT",
  "SDST",
  "ADDIR",
  "LDSPR",
  "SDDEC",
  "SDR",
  "LDR",
};

int disassembleMem();

#endif
