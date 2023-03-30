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
    unsigned int timer_counter = 0;
    long seconds,micros;

	gfx_open(xsize,ysize,"Chip-8");


    chip8 *emulator = chip8_init();

    read_rom(emulator);

    for(;;)
    {
        
        gettimeofday(&start, NULL);
        fetch(emulator);
        decode_execute(emulator);
        gfx_flush();
        
        if(emulator->delay_timer > 0 && timer_counter % 11 == 0)
        {
            emulator->delay_timer--;
        }
        if(emulator->sound_timer > 0 && timer_counter % 11 == 0)
        {
            printf("\a");
            emulator->sound_timer--;
        }

        timer_counter++;
        if(timer_counter >= 0xFFFE) timer_counter = 0;

        gettimeofday(&end, NULL);

        seconds = (end.tv_sec - start.tv_sec);
        micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
        
        microsleep(1200-micros);

    }

	return 0;
}