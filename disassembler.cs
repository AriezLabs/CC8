void my_load() {
  uint64_t fd;
  uint64_t number_of_read_bytes;
  uint64_t i;
  uint64_t j;
  uint64_t iw1;
  uint64_t iw2;
  uint64_t instruction;
  uint64_t address;

  address = 512;
  i = 0;

  binary_name = get_argument();

  fd = sign_extend(open(binary_name, O_RDONLY, 0), SYSCALL_BITWIDTH);

  if (signed_less_than(fd, 0)) {
    printf2("%s: could not open input file %s\n", selfie_name, binary_name);

    exit(EXITCODE_IOERROR);
  }

  // make sure binary is mapped for reading into it
  binary = touch(smalloc(MAX_BINARY_LENGTH), MAX_BINARY_LENGTH);

  number_of_read_bytes = read(fd, binary, MAX_BINARY_LENGTH);

  while (i < number_of_read_bytes) {
    j = 0;

    while (j < 64) {
      iw1 = get_bits(*(binary + (i / 8)), j, 8);
      iw2 = get_bits(*(binary + (i / 8)), j + 8, 8);

      instruction = left_shift(iw1, 8);
      instruction = instruction + iw2;

      print_hexadecimal(address, 0);
      print(": ");
      print_hexadecimal(instruction, 0);
      print("\n");

      address = address + 2;
      j = j + 16;
    }

    i = i + 8;
  }
}

int main(int argc, char** argv) {
  uint64_t brah;
  brah = 12;

  init_selfie((uint64_t) argc, (uint64_t*) argv);

  init_library();

  my_load();
}
