#ifndef CHIP8_H
#define CHIP8_H

#define REGISTERS 16
#define SCREEN_X 64
#define SCREEN_Y 32
#define PIXEL_SHIFTING 75
#define ZOOM_RATE 10
#define STACK_SIZE 10

typedef struct chip8
{
	unsigned short int PC:12; // Program caunter
	unsigned short int I; // Index register 
    unsigned short int opcode; // Opcode
	unsigned char gpr[REGISTERS]; // General purpose register
	unsigned char ram[4096];
    unsigned char screen[SCREEN_Y][SCREEN_X];

}chip8;

typedef struct stack
{
    short int stack[STACK_SIZE];
    short int stack_level;
    short int (*pop) (struct stack *stack_value);
    short int (*push) (struct stack *stack_value, short int value);
    
}stack;


chip8* chip8_init();

int fetch(chip8 *emulator);

void decode_execute(chip8 *emulator);

int read_rom(chip8 *emulator);

short int pop(stack *stack_value);

short int push(stack *stack_value, short int value);



#endif