CC = gcc
CFLAGS = -Wall -Wextra 
LIBS = -lm -lX11

all: main 


gfx.o: gfx.c
	$(CC) $(CFLAGS) $(LIBS) -g -c gfx.c


chip8.o: chip8.c
	$(CC) $(CFLAGS) $(LIBS) -g -c chip8.c


main: gfx.o chip8.o
	$(CC) $(CFLAGS) $(LIBS) -g -o chip8 gfx.o chip8.o main.c



clean_all:
	rm -f test *.o
	rm -f chip8