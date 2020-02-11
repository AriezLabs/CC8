CC = gcc
CCFLAGS = -O0 -g

CC8: CC8.o
	$(CC) $(CCFLAGS) CC8.o colors.o disassembler.o decoder.o -o CC8

colors.o: colors.c colors.h
	$(CC) $(CCFLAGS) -c colors.c

disassembler.o: disassembler.c disassembler.h
	$(CC) $(CCFLAGS) -c disassembler.c

decoder.o: decoder.c decoder.h
	$(CC) $(CCFLAGS) -c decoder.c

CC8.o: colors.o disassembler.o decoder.o CC8.c CC8.h
	$(CC) $(CCFLAGS) -c CC8.c

test: CC8
	./CC8 -d submarine.ch8 > tmp && diff tmp submarine.s && rm tmp
