/* $Id: controls.c,v 1.5 2002/11/01 17:57:18 eric Exp $
 *
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to control controls. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include "types.h"
#include "ship.h" // for the shield_energy variable in the_ship
//#include "msxbios.h"
#include "controls.h"

/*
 * DEFINES
 */

#define NEWKEY 0xFBE5

#define CHECK_KEYBMX(row, col)  !(*((char *)(NEWKEY + row)) & (1<<col))

#define ESC_PRESSED CHECK_KEYBMX(7,2)
#define SHIFT_PRESSED CHECK_KEYBMX(6,0)

void check_quit(char *quit)
{
	*quit=ESC_PRESSED;
}

void check_controls(rotdir_t *rotdir, onoff_t *boost, onoff_t *shield, onoff_t *fire)
{
	static int old_strig;
	int new_strig;
	
	*boost=OFF;
	*shield=OFF;
	*rotdir=ROT_NONE;
	
	switch (gtstck(0))
	{
		case 1: *boost=ON; break;
		case 2: *boost=ON; *rotdir=CLOCKWISE; break;
		case 3: *rotdir=CLOCKWISE; break;
		case 7: *rotdir=COUNTERCLOCKWISE; break;
		case 8: *boost=ON; *rotdir=COUNTERCLOCKWISE; break;
		default: break;
	}

	new_strig = gttrig(0);
	if (new_strig != old_strig)
	{
		old_strig = new_strig;
		if(new_strig)
			*fire = ON;
	}
	else 
		*fire = OFF;
	*shield=(SHIFT_PRESSED || the_ship.shield_energy > AUTOSHIELD_THRESHOLD) 
			&& the_ship.shield_energy > 0;
}
