#include "chip8.h"
#include <stdlib.h>
#include <stdio.h>
#include "gfx.h"
#include <string.h>


chip8* chip8_init()
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

	memcpy(&(emulator->ram[0x50]),fonts,80);



	for(int i=0; i<REGISTERS;i++)
	{
		emulator->gpr[i] = 0;
	}
	emulator->I = 0;
	emulator->PC = 0x200;
	emulator->opcode = 0;

	return emulator;
	
}

int read_rom(chip8 *emulator)
{
	FILE *file = fopen("rom.ch8","r");


	while (!feof(file))
	{
		fread(&(emulator->ram[emulator->PC]),sizeof(char),1,file);
		emulator->PC++;
	}

	emulator->PC = 0x200;

	return 0;
	
}

int fetch(chip8 *emulator)
{

	emulator->opcode =  (emulator->ram[emulator->PC]) << 8 | emulator->ram[emulator->PC+1];
	emulator->PC +=2;

	return 0;
}


void decode_execute(chip8 *emulator)
{
	switch (emulator->opcode & 0xF000)
	{
		case 0x0000: // 00E0 Ekranı temizle 
			gfx_clear();
			break;

		case 0x1000: // 1NNN Satıra atla
			emulator->PC = emulator->opcode & 0x0FFF;
			break;

		case 0x6000: // 6XNN x resgister değerini nn olarak ayarla 
			emulator->gpr[ (emulator->opcode & 0x0F00) >> 8 ] = emulator->opcode & 0x00FF;
			break;

		case 0x7000:// 7xNN x resgister değerine nn'i ekle
			emulator->gpr[ (emulator->opcode & 0x0F00) >> 8 ] += emulator->opcode & 0x00FF;
			break;

		case 0xA000: // ANNN Instructor register değerini ayarla 
			emulator->I = emulator->opcode & 0x0FFF;
			break;

		case 0XD000: // DXYN Ekrana çiz
			unsigned short int x,y,height;
			unsigned char sprite;

			x = (emulator->gpr[(emulator->opcode & 0x0F00) >> 8 ])%64;
			y = (emulator->gpr[(emulator->opcode & 0x00F0) >> 4 ])%32;
			height = emulator->opcode & 0x000F;
			sprite = emulator->ram[emulator->I];
			emulator->gpr[0xF] = 0;


			for(int i=0; i<height;i++)
			{
				for(int j=0x80;j>0; j = j>>1)
				{
					if(sprite & j)
					{
						if(emulator->screen[y][x] == 1)
						{
							gfx_color(0,0,0);
							gfx_point((x+PIXEL_SHIFTING) + x*2,(y+PIXEL_SHIFTING) + y*2);
							gfx_point((x+PIXEL_SHIFTING+1) + x*2,(y+PIXEL_SHIFTING) + y*2); // Nearest-neighbor interpolation
							gfx_point((x+PIXEL_SHIFTING) + x*2,(y+PIXEL_SHIFTING+1) + y*2);
							gfx_point((x+PIXEL_SHIFTING+1) + x*2,(y+PIXEL_SHIFTING+1) + y*2);
							emulator->gpr[0xF] = 1;
							emulator->screen[y][x] = 0;
						}
						else
						{
							gfx_color(255,255,255);
							gfx_point((x+PIXEL_SHIFTING) + x*2,(y+PIXEL_SHIFTING) + y*2);
							gfx_point((x+PIXEL_SHIFTING+1) + x*2,(y+PIXEL_SHIFTING) + y*2); // Nearest-neighbor interpolation
							gfx_point((x+PIXEL_SHIFTING) + x*2,(y+PIXEL_SHIFTING+1) + y*2);
							gfx_point((x+PIXEL_SHIFTING+1) + x*2,(y+PIXEL_SHIFTING+1) + y*2);
							emulator->screen[y][x] = 1;
						}

					}
					x++;
				}
				x = (emulator->gpr[(emulator->opcode & 0x0F00) >> 8 ])%64;
				y++;
				sprite = emulator->ram[emulator->I++];

			}

			break;

		default:
		break;
	}

}

