#ifndef CHIP8_H
#define CHIP8_H

#define REGISTERS 16
#define SCREEN_X 64
#define SCREEN_Y 32
#define PIXEL_SHIFTING 150

typedef struct chip8
{
	unsigned short int PC:12; // Program caunter
	unsigned short int I; // Index register 
    unsigned short int opcode; // Opcode
	unsigned char gpr[REGISTERS]; // General purpose register
	unsigned char ram[4096];
    unsigned char screen[SCREEN_Y][SCREEN_X];

}chip8;

chip8* chip8_init();

int fetch(chip8 *emulator);

void decode_execute(chip8 *emulator);

int read_rom(chip8 *emulator);



#endif