/* $Id: controls.c,v 1.3 2002/09/29 22:44:51 eric Exp $
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
//#include "msxbios.h"
#include "controls.h"

/*
 * DEFINES
 */

#define ESC 27
#define SHIELDKEY ((int)'m')
void check_quit(char *quit)
{
	if (kbhit())
		*quit=(getch()==ESC);
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
	if(kbhit())
		*shield=((getch()|32) == SHIELDKEY);
}
