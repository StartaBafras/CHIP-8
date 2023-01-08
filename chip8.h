#ifndef CHIP8_H
#define CHIP8_H

#define REGISTERS 16

typedef struct chip8
{
	unsigned short int PC:12; // Program caunter
	unsigned short int I; // Index register 
    unsigned short int opcode; // Opcode
	unsigned char gpr[REGISTERS]; // General purpose register
	unsigned char ram[4096]; 


}chip8;

chip8* chip8_init();

int fetch(chip8 *emulator);

int decode(chip8 *emulator);

int read_rom(chip8 *emulator);



#endif