/* $Id: controls.c,v 1.7 2003/02/14 17:52:28 eric Exp $
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
#include "msxbios.h"
#include "ship.h" // for the shield_energy variable in the_ship
#include "controls.h"

/*
 * DEFINES
 */

#define NEWKEY 0xFBE5

#define CHECK_KEYBMX(row, col)  !(*((char *)(NEWKEY + row)) & (1<<col))

#define CHECK_KEYROW(row) ~(*((char *)(NEWKEY + row)))

#define ESC_PRESSED CHECK_KEYBMX(7,2)
#define SHIFT_PRESSED CHECK_KEYBMX(6,0)

char check_quit()
{
	return(ESC_PRESSED);
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

char keypressed()
{
	char pressed=0;
	char row;
	for (row=0; row<11; row++)
		pressed |= CHECK_KEYROW(row);
	return(pressed);
}	
