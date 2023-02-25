#include "gfx.h"
#include "chip8.h"
#include<time.h>
#include <unistd.h>


int main()
{
    
	int ysize = 600;
	int xsize = 600;
    clock_t start, stop;


	// Open a new window for drawing.
	gfx_open(xsize,ysize,"Chip-8");

	// Set the current drawing color to green.
	gfx_color(0,200,100);
    //gfx_point(0,0);
    //gfx_point(300,300);
    //gfx_point(150,150);
    //gfx_flush();
    
	// Draw a triangle on the screen.

    chip8 *emulator = chip8_init();

    read_rom(emulator);

    for(;;)
    {
        start = clock();
        fetch(emulator);
        decode_execute(emulator);
        gfx_flush();
        stop = clock();
        sleep( (16-(start-stop))/1000); //62.5 Hz

    }

	return 0;
}