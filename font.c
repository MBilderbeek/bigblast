/* $Id: font.c,v 1.1 2002/12/26 23:54:11 manuel Exp $
 *
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains the fonty functions. Yup. That's it.
 */

/*
 * INCLUDES
 */
#include "string.h"
#include "glib.h"
#include "renderer.h"
#include "font.h"

/*
 * LOCAL DEFINITIONS
 */

#define FONT_Y 188
#define FONT_CPL 42 // Nof characters on a line

void write_cent(char *string, unsigned int y)
{
	write(string, (255-(strlen(string)*FONT_W)) >> 1, y);
}

void write(char *string, unsigned int x, unsigned int y)
{
	unsigned char i,charnumber,sx,sy;
	
	for (i=0; i<strlen(string); i++)
	{
		if (string[i]>=32 && string[i]<=127)
		{
			charnumber=(string[i]-32);
			sx=(charnumber%FONT_CPL)*FONT_W;
			sy=FONT_Y+(charnumber/FONT_CPL)*FONT_H;
			cpyv2v(sx, sy, sx+FONT_W-1, sy+FONT_H-1, GFXPAGE,
					x+i*FONT_W, y, c_apage, PSET);
		}
	}
}
