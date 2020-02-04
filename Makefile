emu: CC8.o
	gcc CC8.o colors.o -o CC8

colors.o: colors.c colors.h
	gcc -c colors.c

CC8.o: colors.o CC8.c
	gcc -c CC8.c
