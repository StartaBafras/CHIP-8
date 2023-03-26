#include "gfx.h"
#include "chip8.h"
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>





int main()
{
    struct timeval start, end;

	int ysize = 500;
	int xsize = 800;




	gfx_open(xsize,ysize,"Chip-8");


    chip8 *emulator = chip8_init();

    read_rom(emulator);

    for(;;)
    {
        
        gettimeofday(&start, NULL);
        fetch(emulator);
        decode_execute(emulator);
        gfx_flush();
        msleep(1);
        //msleep((16,666666667-(micros*1000))); //62.5 Hz
        gettimeofday(&end, NULL);

        long seconds = (end.tv_sec - start.tv_sec);
        long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
        
        printf(" %d seconds %d micros\n", seconds, micros);

        
        if(emulator->delay_timer > 0)
        {
            emulator->delay_timer--;
        }
        if(emulator->sound_timer > 0)
        {
            printf("\a");
            emulator->sound_timer--;
        }
    }

	return 0;
}