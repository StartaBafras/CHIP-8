CC = gcc
CFLAGS = -Wall -Wextra 
LIBS = -lm -lX11

all: chip8 


gfx.o: gfx.c
	$(CC) $(CFLAGS) $(LIBS) -g -c gfx.c


chip8: gfx.o 
	$(CC) $(CFLAGS) $(LIBS) -o chip8 gfx.o chip8.c


clean_s:
	rm -f *.s

clean_all:
	rm -f test *.o
	rm -f *.s