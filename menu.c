/* $Id: menu.c,v 1.4 2004/08/22 20:01:26 eric Exp $
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

#define MENU_BASE_X 104
#define MENU_BASE_Y 162 
#define MENU_ITEM_H 13
#define MENU_ITEM_W 46 
#define MENU_ITEM_DIST 7 

#define NOF_ITEMS 2

#define C_CURSOR 15

extern int gs2loadgrp(char *,  uchar);
extern int gs2loadpal(char *, int *);

void draw_cursor(menu_item_e curpos)
{
	boxfill(MENU_BASE_X-1, 
			MENU_BASE_Y-1 + curpos*(MENU_ITEM_H+MENU_ITEM_DIST),
		MENU_BASE_X+1 + MENU_ITEM_W, 
		MENU_BASE_Y+1 + MENU_ITEM_H + 
					curpos*(MENU_ITEM_H+MENU_ITEM_DIST),
		C_CURSOR,XOR);
}

void init_menu()
{
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
