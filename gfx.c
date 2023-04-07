/*
A simple graphics library for CSE 20211 by Douglas Thain
For complete documentation, see:
http://www.nd.edu/~dthain/courses/cse20211/fall2011/gfx
version 4, 01/29/2020 - Added missing window size functions and fixed key lookup.
Version 4, 01/20/2020 - Added missing window size functions.
Version 3, 11/07/2012 - Now much faster at changing colors rapidly.
Version 2, 9/23/2011 - Fixes a bug that could result in jerky animation.
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "gfx.h"

/*
gfx_open creates several X11 objects, and stores them in globals
for use by the other functions in the library.
*/

static Display *gfx_display=0;
static Window  gfx_window;
static GC      gfx_gc;
static Colormap gfx_colormap;
static int      gfx_fast_color_mode = 0;

static XImage *gfx_image; // For get pixel value

/* These values are saved by gfx_wait then retrieved later by gfx_xpos and gfx_ypos. */

static int saved_xpos = 0;
static int saved_ypos = 0;
static int saved_xsize = 0;
static int saved_ysize = 0;

/* Open a new graphics window. */

void gfx_open( int width, int height, const char *title )
{
	gfx_display = XOpenDisplay(0);
	if(!gfx_display) {
		fprintf(stderr,"gfx_open: unable to open the graphics window.\n");
		exit(1);
	}

	Visual *visual = DefaultVisual(gfx_display,0);
	if(visual && visual->class==TrueColor) {
		gfx_fast_color_mode = 1;
	} else {
		gfx_fast_color_mode = 0;
	}

	int blackColor = BlackPixel(gfx_display, DefaultScreen(gfx_display));
	int whiteColor = WhitePixel(gfx_display, DefaultScreen(gfx_display));

	gfx_window = XCreateSimpleWindow(gfx_display, DefaultRootWindow(gfx_display), 0, 0, width, height, 0, blackColor, blackColor);

	XSetWindowAttributes attr;
	attr.backing_store = Always;

	XChangeWindowAttributes(gfx_display,gfx_window,CWBackingStore,&attr);

	XStoreName(gfx_display,gfx_window,title);

	XSelectInput(gfx_display, gfx_window, StructureNotifyMask|KeyPressMask|ButtonPressMask);

	XMapWindow(gfx_display,gfx_window);

	gfx_gc = XCreateGC(gfx_display, gfx_window, 0, 0);

	gfx_colormap = DefaultColormap(gfx_display,0);

	XSetForeground(gfx_display, gfx_gc, whiteColor);

	// Wait for the MapNotify event

	for(;;) {
		XEvent e;
		XNextEvent(gfx_display, &e);
		if (e.type == MapNotify)
			break;
	}

	saved_xsize = width;
	saved_ysize = height;
}

/* Draw a single point at (x,y) */

void gfx_point( int x, int y )
{
	XDrawPoint(gfx_display,gfx_window,gfx_gc,x,y);
}

/* Draw a line from (x1,y1) to (x2,y2) */

void gfx_line( int x1, int y1, int x2, int y2 )
{
	XDrawLine(gfx_display,gfx_window,gfx_gc,x1,y1,x2,y2);
}

/* Change the current drawing color. */

void gfx_color( int r, int g, int b )
{
	XColor color;

	if(gfx_fast_color_mode) {
		/* If this is a truecolor display, we can just pick the color directly. */
		color.pixel = ((b&0xff) | ((g&0xff)<<8) | ((r&0xff)<<16) );
	} else {
		/* Otherwise, we have to allocate it from the colormap of the display. */
		color.pixel = 0;
		color.red = r<<8;
		color.green = g<<8;
		color.blue = b<<8;
		XAllocColor(gfx_display,gfx_colormap,&color);
	}

	XSetForeground(gfx_display, gfx_gc, color.pixel);
}

/* Clear the graphics window to the background color. */

void gfx_clear()
{
	XClearWindow(gfx_display,gfx_window);
}

/* Change the current background color. */

void gfx_clear_color( int r, int g, int b )
{
	XColor color;
	color.pixel = 0;
	color.red = r<<8;
	color.green = g<<8;
	color.blue = b<<8;
	XAllocColor(gfx_display,gfx_colormap,&color);

	XSetWindowAttributes attr;
	attr.background_pixel = color.pixel;
	XChangeWindowAttributes(gfx_display,gfx_window,CWBackPixel,&attr);
}

int gfx_event_waiting()
{
       XEvent event;

       gfx_flush();

       while (1) {
               if(XCheckMaskEvent(gfx_display,-1,&event)) {
                       if(event.type==KeyPress) {
                               XPutBackEvent(gfx_display,&event);
                               return 1;
                       } else if (event.type==ButtonPress) {
                               XPutBackEvent(gfx_display,&event);
                               return 1;
                       } else {
                               return 0;
                       }
               } else {
                       return 0;
               }
       }
}

/* Wait for the user to press a key or mouse button. */

int gfx_wait()
{
	XEvent event;

	gfx_flush();

	while(1) {
		XNextEvent(gfx_display,&event);

		if(event.type==KeyPress) {
			saved_xpos = event.xkey.x;
			saved_ypos = event.xkey.y;

			/* If the key sequence maps to an ascii character, return that. */
			KeySym symbol;
			char str[4];
			int r = XLookupString(&event.xkey,str,sizeof(str),&symbol,0);
			if(r==1) return str[0];

			/* Special case for navigation keys, return codes above 129. */
			if(symbol>=0xff50 && symbol<=0xff58) {
				return 129 + (symbol-0xff50);
			}

		} else if(event.type==ButtonPress) {
			saved_xpos = event.xkey.x;
			saved_ypos = event.xkey.y;
			return event.xbutton.button;
		} else if(event.type==ConfigureNotify) {
			saved_xsize = event.xconfigure.width;
			saved_ysize = event.xconfigure.height;
		}
	}
}

/* Return the X and Y coordinates of the last event. */

int gfx_xpos()
{
	return saved_xpos;
}

int gfx_ypos()
{
	return saved_ypos;
}

/* Flush all previous output to the window. */

void gfx_flush()
{
	XFlush(gfx_display);
}

int gfx_xsize()
{
	return saved_xsize;
}

int gfx_ysize()
{
	return saved_ysize;
}

//Added later not included in original gfx

/*Return pixel value in 24 bits */

int get_pixel(int x,int y)
{
	gfx_image = XGetImage(gfx_display,gfx_window,x,y,1,1,AllPlanes,XYPixmap);
    return XGetPixel(gfx_image,0,0);
}

/* Converts keyboard inputs to chip-8 inputs */

int convert_key(int key_value)
{
	switch (key_value)
	{
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 0xC;
		case 'q':
			return 4;
		case 'w':
			return 5;
		case 'e':
			return 6;
		case 'r':
			return 0xD;
		case 'a':
			return 7;
		case 's':
			return 8;
		case 'd':
			return 9;
		case 'f':
			return 0xE;
		case 'z':
			return 0xA;
		case 'x':
			return 0;
		case 'c':
			return 0xB;
		case 'v':
			return 0xF;
		default:
			return -1;
			break;
	
	}
}


/* The difference from the gfx_wait function is that it 
reads events from the Xlib queue without removing them, 
and only checks for keyboard input and sends it, without 
handling any other events.
*/

int get_key()
{
	XEvent event;
	XPeekEvent(gfx_display,&event);

	if(event.type==KeyPress) 
	{
		saved_xpos = event.xkey.x;
		saved_ypos = event.xkey.y;

		/* If the key sequence maps to an ascii character, return that. */
		KeySym symbol;
		char str[4];
		int r = XLookupString(&event.xkey,str,sizeof(str),&symbol,0);
		if(r==1) return str[0];

		/* Special case for navigation keys, return codes above 129. */
		if(symbol>=0xff50 && symbol<=0xff58) 
		{
			return 129 + (symbol-0xff50);
		}

	}
	
	return -1;
}


/* Returns the number of events in the event queue. */

int check_queue()
{
	return XPending(gfx_display);
}


/* Removes the event that is waiting in the queue. */
int remove_event()
{
	XEvent event;
	if(XPending(gfx_display))
		XNextEvent(gfx_display, &event);
	
	return 0;

}

/*Takes an event from the event queue, allowing only one event to be present in the queue. 
If there are more events, it discards them.*/
int get_keyboard_event()
{
	XEvent event;

	while (XEventsQueued(gfx_display, QueuedAlready) > 0) 
	{
    	XNextEvent(gfx_display, &event);
    	if (event.type == KeyPress) 
		{
        	if (XEventsQueued(gfx_display, QueuedAlready) > 0) 
			{
            	XEvent next_event;
            	XPeekEvent(gfx_display, &next_event);
            	if (next_event.type == KeyPress) 
				{
                	XNextEvent(gfx_display, &next_event);
            	}
        	}
			else XPutBackEvent(gfx_display, &event);
        	break;
    	}
	}

	return 0;
}