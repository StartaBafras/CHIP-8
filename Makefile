CC = gcc
CFLAGS = -Wall -Wextra 
LIBS = -lm -lX11

all: chip8 


gfx.o: gfx.c
	$(CC) $(CFLAGS) $(LIBS) -g -c gfx.c


chip8.o: chip8.c
	$(CC) $(CFLAGS) $(LIBS) -g -c chip8.c

main.o: main.c
	$(CC) $(CFLAGS) $(LIBS) -g -c main.c


chip8: gfx.o chip8.o main.o
	$(CC) $(CFLAGS) $(LIBS) -g -o chip8 gfx.o chip8.o main.o



clean_all:
	rm -f test *.o
	rm -f chip8