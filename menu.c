/* $Id: menu.c,v 1.2 2003/02/16 15:34:01 eric Exp $
 *
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains the menu functions. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include "msxbios.h"
#include "glib.h"
#include "font.h"
#include "menu.h"
#include "controls.h"

/*
 * LOCAL DEFINITIONS
 */

#define MENU_BASE_X 100
#define MENU_BASE_Y 150 
#define MENU_ITEM_H (FONT_H+2)

#define NOF_ITEMS 2

#define C_CURSOR 15

static int palette[] = 
{	/*-GRB*/
	0x0000,
	0x0111, /* 1: black    */
	0x0777, /* 2:          */
	0x0404, /* 3:          */
	0x0202, /* 4:          */
	0x0333, /* 5:          */
	0x0070, /* 6:          */
	0x0527, /* 7:          */
	0x0527, /* 8:          */
	0x0572, /* 9:          */
	0x0333, /*10:          */
	0x0333, /*11:          */
	0x0333, /*12:          */
	0x0333, /*13:          */
	0x0117, /*14:          */
	0x0777  /*15: cursor   */
};

void draw_cursor(menu_item_e curpos)
{
	boxfill(MENU_BASE_X-1, MENU_BASE_Y - 1 + curpos*MENU_ITEM_H,
		MENU_BASE_X+1 + 4*FONT_W - 1, 
		MENU_BASE_Y+1 + FONT_H - 1 + curpos*MENU_ITEM_H,
		C_CURSOR,XOR);
}

void init_menu()
{
	uchar i;
	char string[100];
	for(i=0; i<16;i++)
	{
		setplt(i, palette[i]);
	}

	// print the menu options
	sprintf(string,"Play");
	write(string, MENU_BASE_X, MENU_BASE_Y);
	sprintf(string,"Quit");
	write(string, MENU_BASE_X, MENU_BASE_Y+10);
	
	// draw cursor for first item
	draw_cursor(PLAY);
}

menu_item_e menu_select()
{
	int old_stick=0, new_stick=0;
	menu_item_e curpos, newcurpos;
	char done=0;
	
	do
	{
		new_stick = gtstck(0);
		if (new_stick != old_stick)
		{
			old_stick = new_stick;
			switch (new_stick)
			{
				case 1: newcurpos=(curpos+NOF_ITEMS-1)%NOF_ITEMS;
					break;
				case 5: newcurpos=(curpos+1)%NOF_ITEMS; 
					break;
				default: ;
			}
			if (newcurpos!=curpos)
			{
				draw_cursor(curpos);
				curpos=newcurpos;
				draw_cursor(curpos);
			}
		}
		done = gttrig(0);
		if (check_quit()) 
		{ 
			done = 1;
			curpos = QUIT;
		}
	}
	while (!done);
	
	kilbuf();
	return(curpos);
}
