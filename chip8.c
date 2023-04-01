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

	memcpy(&(emulator->ram[0]), fonts, 80);

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
	FILE *file = fopen("Tetris [Fran Dachille, 1991].ch8", "r");

	if(file == NULL) 
	{
		printf("Rom okuma hatası.\n");
		exit(1);
	}

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
	case 0x0000: 

		switch (emulator->opcode & 0x00FF)
		{
			case 0x00E0: // 00E0 Ekranı temizle
				gfx_clear();
				for(int i=0; i<SCREEN_Y; i++)
				{
					for(int j=0; j<SCREEN_X; j++)
					{
						emulator->screen[i][j] = 0;
					}
				}
				
				break;

			case 0x00EE: // 00EE Altrutinler, yığında sırada bulunulan adrese atla
				emulator->PC = 0X0FFF & emulator->chip_stack->pop(emulator->chip_stack);
				break;

			default:
				printf("Hatalı buyruk \n");
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
			case 0x0000: // 8XY0 VX, VY değerine atanır
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				break;

			case 0x0001: // 8XY1 VX, VX or VY sonucuna atanır
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] |= emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				break;

			case 0x0002: // 8XY2 VX, VX and VY sonucuna atanır
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] &= emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				break;

			case 0x0003: // 8XY3 VX, VX xor VY sonucuna atanır
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] ^= emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				break;

			case 0x0004: // 8XY4 VX, VX + VY sonucuna atanır
				if(emulator->gpr[(emulator->opcode & 0x0F00) >> 8] + emulator->gpr[(emulator->opcode & 0x00F0) >> 4] > 255)
				{
					emulator->gpr[0xF] = 1;
				}
				else emulator->gpr[0xF] = 0;

				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] += emulator->gpr[(emulator->opcode & 0x00F0) >> 4];

				break;

			case 0x0005: // 8XY5 VX, VX - VY sonucuna atanır
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] -= emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				break;

			case 0x0006: // Kontrol eklenecek 8XY6 VX, 1 bit sağa kaydırılır ve kaydırılan (kaybloan) bitin değeri kayıt edilir
				//emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				emulator->gpr[0xF] = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] & 0x0001;
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] >> 1;

				break;

			case 0x0007: // 8XY7 VX, VY - VX sonucuna atanır
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x00F0) >> 4] - emulator->gpr[(emulator->opcode & 0x0F00) >> 8];
				break;
			
			case 0x000E: // Kontrol eklenecek 8XYE VX, 1 bit sola kaydırılır ve kaydırılan (kaybloan) bitin değeri kayıt edilir
				//emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x00F0) >> 4];
				emulator->gpr[0xF] = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] & 0x80;
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] << 1;
				
				break;

			default:
				printf("Hatalı buyruk\n");
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
	
	case 0xB000: // BNNN Program sayacı, NNN + 0. yazmaçın değerine atlar
		emulator->PC = (emulator->opcode & 0x0FFF) + emulator->gpr[0];
		break;

	case 0xC000: // CXNN 0-NN arasında rastgele bir sayı üretir ve VX yazmacında depolar 
		emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = (rand() % 255) & (emulator->opcode & 0x00FF);
		break;


	case 0XD000: // DXYN Ekrana çiz
		unsigned short int x, y, height;
		unsigned char sprite;

		x = (emulator->gpr[(emulator->opcode & 0x0F00) >> 8]) % 64;
		y = (emulator->gpr[(emulator->opcode & 0x00F0) >> 4]) % 32;
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
						gfx_flush();
						emulator->gpr[0xF] = 1;
						emulator->screen[y][x] = 0;
					}
					else
					{
						gfx_color(255, 255, 255);
						Nearest_neighbor_interpolation(ZOOM_RATE, x, y);
						gfx_flush();
						emulator->screen[y][x] = 1;
					}
				}
				x = (x+1) % 64;
			}

			x = (emulator->gpr[(emulator->opcode & 0x0F00) >> 8]) % 64;
			y = (y+1) % 32;

		}

		break;

	case 0xE000:
		switch (emulator->opcode & 0x00FF)
		{
			case 0x009E: // EX9E Bir tuşa basılıyorsa ve bu tuş VX'e eşitse 1 buyruk atla
				if(gfx_event_waiting())
				{
					char c = convert_key(gfx_wait());
					if(c == emulator->gpr[(emulator->opcode & 0x0F00) >> 8])
					{
						emulator->PC += 2;
					}
				}
				break;
			
			case 0x00A1: // EXA1 Bir tuşa basılıyorsa ve bu tuş VX'e eşit değilse 1 buyruk atla
				if(gfx_event_waiting())
				{
					char c = convert_key(gfx_wait());
					if(c != emulator->gpr[(emulator->opcode & 0x0F00) >> 8])
					{
						emulator->PC += 2;
					}
				}
				else
				{
					emulator->PC += 2;
				} 
				break;

			default:
				printf("Hatalı buyruk\n");
				break;
		}
		break;
	case 0xF000:
		switch (emulator->opcode & 0x00FF)
		{
			case 0x0007: // FX07 VX'i gecikme zamanlayıcısının geçerli değerine ayarlar
				emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = emulator->delay_timer;
				break;

			case 0x0015: // FX15 Gecikme zamanlayıcısını VX'teki değere ayarlar
				emulator->delay_timer = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] ;
				break;

			case 0x0018: // FX18 ses zamanlayıcısını VX'teki değere ayarlar
				emulator->sound_timer = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] ;
				break;

			case 0x001E: // FX1E Instructor register değeri ile VX değerini topla
				emulator->I += emulator->gpr[(emulator->opcode & 0x0F00) >> 8];
				//if(emulator->I > 1000) emulator->I = 1;
				break;

			case 0x000A: // FX0A Bir tuş bekle ve bunu VX'e ekle
				emulator->PC -= 2;
				if(gfx_event_waiting())
				{
					emulator->PC += 2;
					char c = convert_key(gfx_wait());
					emulator->gpr[(emulator->opcode & 0x0F00) >> 8] = c;

				}
				break;

			case 0x0029: // FX29 Instructor register değeri VX içerisinde depolanan karekterin çizilmesi için depolandığı adrese atlatılır
				emulator->I = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] * 5;
				break;

			case 0x0033: // FX33 VX içindeki değeri Instructor yazmacının işaret ettiği yere basamak basamak ondalık sayı olarak depolanır
				emulator->ram[emulator->I] = emulator->gpr[(emulator->opcode & 0x0F00) >> 8] / 100;
				emulator->ram[emulator->I+1] = (emulator->gpr[(emulator->opcode & 0x0F00) >> 8] / 10) % 10;
				emulator->ram[emulator->I+2] = (emulator->gpr[(emulator->opcode & 0x0F00) >> 8] % 100) %10;
				break;

			case 0x0055: // FX33 0'dan X'e kadar olan sayıdaki yazmaçları ardaşık olarak ram üzerine yazar
				for(int i=0; i <= (emulator->opcode & 0x0F00) >> 8 ; i++)
				{
					emulator->ram[emulator->I+i] = emulator->gpr[i];
				}
				break;

			case 0x0065: // FX65 0'dan X'e kadar olan sayıdaki yazmaçları ardaşık ram üzerindeki bilgileri kayıt eder
				for(int i=0; i <= (emulator->opcode & 0x0F00) >> 8 ; i++)
				{
					emulator->gpr[i] = emulator->ram[emulator->I+i];
				}
				break;


			default:
				printf("Hatalı buyruk\n");
				break;
		}

		break;
		
	default:
		printf("Hatalı buyruk\n");
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

int microsleep(long microsec)
{
    struct timespec ts;

    if (microsec < 0) return -1;


    ts.tv_sec = microsec / 1000000;
    ts.tv_nsec = (microsec % 1000000) * 1000;
    
    nanosleep(&ts, &ts);

    return 0;
}