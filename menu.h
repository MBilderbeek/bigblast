/* $Id: menu.h,v 1.1 2002/12/26 23:54:11 manuel Exp $
 *
 * AUTHOR(S)   : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   interface definition for the menu functions.
 */

#ifndef MENU_H
#define MENU_H

/*
 * INCLUDES
 */


/*
 * DEFINITIONS
 */

typedef enum 
{
	PLAY,
	QUIT
} menu_item_e;


extern void init_menu();
extern menu_item_e menu_select();

#endif

