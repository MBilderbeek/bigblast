/* $Id: explosio.c,v 1.1 2003/02/07 01:38:46 manuel Exp $
 * 
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to handle the explosions. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include "stdlib.h"
#include "object.h"
#include "explosio.h"

/*
 * LOCAL DEFINITIONS
 */

explosion_t the_explosions[MAX_NOF_EXPLOSIONS];

/*
 * EXTERNAL FUNCTIONS
 */

void explosions_init()
{
	exp_hdl_t i;
	
	for (i=0; i<MAX_NOF_EXPLOSIONS; i++)
	{
		the_explosions[i].size = EXP_NONE;
		the_explosions[i].age = 0;		
		the_explosions[i].explosion_obj = OBJ_VOID;
	}
	//nof_explosions=0;
}

void explosions_move()
{
	exp_hdl_t i;

	for (i=0; i<MAX_NOF_EXPLOSIONS; i++)
	{
		if (the_explosions[i].age > 0)
		{
			object_move(the_explosions[i].explosion_obj);
			the_explosions[i].age--;
			if (the_explosions[i].age == 0)
			    object_set_state(the_explosions[i].explosion_obj, 
						 DYING);
		}
		else if (object_get_state(the_explosions[i].explosion_obj) == DYING)
			object_destroy(&(the_explosions[i].explosion_obj));
	}
}

exp_hdl_t explosion_create(int x, int y, expsize_e size, int dx, int dy)
{
	exp_hdl_t i;

	for (i=0; i<MAX_NOF_EXPLOSIONS; i++)
	{
		if (the_explosions[i].explosion_obj == OBJ_VOID)
		{
			the_explosions[i].explosion_obj = 
				object_create (x, y, dx, dy, OBJ_EXP);
			the_explosions[i].size = size;
			the_explosions[i].age = NEW_EXPLOSION_AGE;
			break;
		}
	}

	/* gaat altijd goed (waaaah) */

	return i;
}

void explosion_destroy(exp_hdl_t exp)
{
	object_destroy(&(the_explosions[exp].explosion_obj));
	the_explosions[exp].size = EXP_NONE;
}

