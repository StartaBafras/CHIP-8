#include "chip8.h"
#include <stdlib.h>
#include <stdio.h>
#include "gfx.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>


chip8 *chip8_init()
{
	chip8 *emulator = malloc(sizeof(chip8));

	unsigned char fonts[80] =
		{
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		};

	memcpy(&(emulator->ram[0x50]), fonts, 80);

	for (int i = 0; i < REGISTERS; i++)
	{
		emulator->gpr[i] = 0;
	}
	emulator->I = 0;
	emulator->PC = 0x200;
	emulator->opcode = 0;
	emulator->delay_timer = 0;
	emulator->sound_timer = 0;

	emulator->chip_stack = stack_generator();

	srand(time(NULL));

	return emulator;
}

int read_rom(chip8 *emulator)
{
	FILE *file = fopen("Zero Demo [zeroZshadow, 2007].ch8", "r");

	while (!feof(file))
	{
		fread(&(emulator->ram[emulator->PC]), sizeof(char), 1, file);
		emulator->PC++;
	}

	emulator->PC = 0x200;

	return 0;
}

int fetch(chip8 *emulator)
{

	emulator->opcode = (emulator->ram[emulator->PC]) << 8 | emulator->ram[emulator->PC + 1];
	emulator->PC += 2;

	return 0;
}

int Nearest_neighbor_interpolation(int zoom_rate, int x, int y) // Nearest-neighbor interpolation
{
	for (int i = 0; i < zoom_rate; i++)
	{
		for (int j = 0; j < zoom_rate; j++)
		{
			gfx_point((x * zoom_rate) - (zoom_rate - 1) + PIXEL_SHIFTING + i, (y * zoom_rate) - (zoom_rate - 1) + PIXEL_SHIFTING + j);
		}
	}

	// x2 zoom example
	/*
		gfx_point((x*2)-1+PIXEL_SHIFTING,  (y*2)-1+PIXEL_SHIFTING);
		gfx_point((x*2)-1+PIXEL_SHIFTING+1,(y*2)-1+PIXEL_SHIFTING);
		gfx_point((x*2)-1+PIXEL_SHIFTING, (y*2)-1+PIXEL_SHIFTING+1);
		gfx_point((x*2)-1+PIXEL_SHIFTING +1, (y*2)-1+PIXEL_SHIFTING+1);
	*/

	return 0;
}

void decode_execute(chip8 *emulator)
{
	switch (emulator->opcode & 0xF000)
	{
	case 0x0000: // 00E0 Ekranı temizle

		switch (emulator->opcode & 0x00FF)
		{
		case 0x00E0:
			gfx_clear();
			break;

		case 0x00EE:
			emulator->PC = 0X0FFF & emulator->chip_stack->pop(emulator->chip_stack);

		default:
			break;
		}

		break;

	case 0x1000: // 1NNN Satıra atla
		emulator->PC = emulator->opcode & 0x0FFF;
		break;

	case 0x2000: // 2NNN Satıra atla ve mevcut konumu kayıt et
		emulator->chip_stack->push(emulator->chip_stack, emulator->PC);
		emulator->PC = emulator->opcode & 0x0FFF;
		break;

	case 0x3000: // 3XNN x ve NN eşitse 1 komut atla
		if(emulator->gpr[(emulator->opcode & 0x0F00) >> 8] == (emulator->opcode & 0x00FF))
		{
			emulator->PC += 2;
		}
		break;

	case 0x4000: // 4XNN x ve NN eşit değilse 1 komut atla
		if(emulator->gpr[(emulator->opcode & 0x0F00) >> 8] != (emulator->opcode & 0x00FF))
		{
			emulator->PC += 2;
		}
		break;

	case 0x5000: // 5XY0 x ve y eşitse 1 komut atla
		if(emulator->gpr[(emulator->opcode & 0x0F00) >> 8] == emulator->gpr[(emulator->opcode & 0x00F0) >> 4])
		{
			emulator->PC += 2;
		}
		break;

	case 0x6000: // 6XNN x resgister değerini nn olarak ayarla
		emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->opcode & 0x00FF;
		break;

	case 0x7000: // 7XNN x resgister değerine nn'i ekle
		emulator->gpr[(emulator->opcode & 0x0F00) >> 8] += emulator->opcode & 0x00FF;
		break;

	case 0x8000:

		switch (emulator->opcode & 0x000F)
		{
			case 0x0000:
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				break;

			case 0x0001:
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] |= emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				break;

			case 0x0002:
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] &= emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				break;

			case 0x0003:
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] ^= emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				break;

			case 0x0004:
				if(emulator->gpr[(emulator->opcode & 0x0F00) >> 8] + emulator->gpr[(emulator->opcode & 0x00F0) >> 4] > 255)
				{
					emulator->gpr[0xF] = 1;
				}
				else emulator->gpr[0xF] = 1;


				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] += emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				break;

			case 0x0005:
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] -= emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				break;

			case 0x0006: // Kontrol eklenecek
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] >> 1;
				emulator->gpr[0xF] = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] & 0x0002;
				break;

			case 0x0007:
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x00F0) >> 4] - emulator->gpr[(emulator->opcode & 0x0F00) >> 8];
				break;
			
			case 0x000E: // Kontrol eklenecek
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] << 1;
				emulator->gpr[0xF] = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] & 0x0002;
				break;

			default:
				break;
		}
	
		break;

	case 0x9000: // 9XY0 x ve y eşit değilse 1 komut atla
		if(emulator->gpr[(emulator->opcode & 0x0F00) >> 8] != emulator->gpr[(emulator->opcode & 0x00F0) >> 4])
		{
			emulator->PC += 2;
		}
		break;

	case 0xA000: // ANNN Instructor register değerini ayarla
		emulator->I = emulator->opcode & 0x0FFF;
		break;
	
	case 0xB000:
		emulator->I = (emulator->opcode & 0x0FFF) + emulator->gpr[0];
		break;

	case 0xC000:
		emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = rand() % (emulator->opcode & 0x00FF);
		break;


	case 0XD000: // DXYN Ekrana çiz
		unsigned short int x, y, height;
		unsigned char sprite;

		x = (emulator->gpr[(emulator->opcode & 0x0F00) >> 8]);
		y = (emulator->gpr[(emulator->opcode & 0x00F0) >> 4]);
		height = emulator->opcode & 0x000F;
		sprite = emulator->ram[emulator->I];
		emulator->gpr[0xF] = 0;


		for (int i = 0; i < height; i++)
		{
			sprite = emulator->ram[emulator->I+i];
			for (int j = 0x80; j > 0; j = j >> 1)
			{
				if (sprite & j)
				{
					if (emulator->screen[y][x] == 1)
					{
						gfx_color(0, 0, 0);
						Nearest_neighbor_interpolation(ZOOM_RATE, x, y);
						emulator->gpr[0xF] = 1;
						emulator->screen[y][x] = 0;
					}
					else
					{
						gfx_color(255, 255, 255);
						Nearest_neighbor_interpolation(ZOOM_RATE, x, y);
						emulator->screen[y][x] = 1;
					}
				}
				x++;
			}

			x = (emulator->gpr[(emulator->opcode & 0x0F00) >> 8]) % 64;
			y++;

		}

		break;

	case 0xE000:
		switch (emulator->opcode & 0x00FF)
		{
			case 0x009E:
				if(gfx_event_waiting())
				{
					char c = convert_key(gfx_wait());
					if(c == emulator->gpr[(emulator->opcode & 0x0F00) >> 8])
					{
						emulator->PC += 4;
					}
				}
				break;
			
			case 0x00A1:
				if(gfx_event_waiting())
				{
					char c = convert_key(gfx_wait());
					if(c != emulator->gpr[(emulator->opcode & 0x0F00) >> 8])
					{
						emulator->PC += 4;
					}
				}
				break;

			default:
				break;
		}
		break;
	case 0xF000:
		switch (emulator->opcode & 0x00FF)
		{
			case 0x0007:
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->delay_timer;
				break;

			case 0x0015:
				emulator->delay_timer = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] ;
				break;

			case 0x0018:
				emulator->sound_timer = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] ;
				break;

			case 0x001E:
				emulator->I += emulator->gpr[(emulator->opcode & 0x0F00) >> 8];
				if(emulator->I > 1000) emulator->I = 1;
				break;

			case 0x000A:
				emulator->PC -= 2;
				if(gfx_event_waiting())
				{
					emulator->PC += 2;
					char c = convert_key(gfx_wait());
					emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = c;

				}
				break;

			case 0x0029:
				emulator->I = emulator->gpr[(emulator->opcode & 0x0F00) >> 8];
				break;

			case 0x0033:
				emulator->ram[emulator->I] = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] / 100;
				emulator->ram[emulator->I+1] = (emulator->gpr[(emulator->opcode & 0x0F00) >> 8] / 10) % 10;
				emulator->ram[emulator->I+2] = (emulator->gpr[(emulator->opcode & 0x0F00) >> 8] % 100) %10;
				break;

			case 0x0055:
				for(int i=0; i <= (emulator->opcode & 0x0F00) >> 8 ; i++)
				{
					emulator->ram[emulator->I+i] = emulator->gpr[i];
				}
				break;

			case 0x0065:
				for(int i=0; i <= (emulator->opcode & 0x0F00) >> 8 ; i++)
				{
					emulator->gpr[i] = emulator->ram[emulator->I+i];
				}
				break;


			default:
				break;
		}

		break;
		
	default:
		break;
	}
}

short int pop(stack *stack_value)
{
	short int value = stack_value->stack[stack_value->stack_level];
	stack_value->stack[stack_value->stack_level] = 0;
	stack_value->stack_level--;
	return value;
}

short int push(stack *stack_value, short int value)
{
	stack_value->stack_level++;
	stack_value->stack[stack_value->stack_level] = value;
	return 0;
}

stack *stack_generator()
{
	stack *new_stack = malloc(sizeof(stack));
	new_stack->pop = &pop;
	new_stack->push = &push;
	new_stack->stack_level = -1;

	return new_stack;
}