/* $Id: explosio.c,v 1.4 2003/02/16 15:34:01 eric Exp $
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

#define EXP_BIG_SIZE  (16 << OBJ_GFX_FACTOR)
#define EXP_MED_SIZE  (10 << OBJ_GFX_FACTOR)
#define EXP_SMALL_SIZE (6 << OBJ_GFX_FACTOR)

explosion_t the_explosions[MAX_NOF_EXPLOSIONS];
char nof_explosions;

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
	nof_explosions=0;
}

void explosions_move()
{
	exp_hdl_t i;
	char counter=0;

	for (i=0; i<MAX_NOF_EXPLOSIONS && counter<=nof_explosions; i++)
	{
		if (the_explosions[i].age > 0)
		{
			counter++;
			object_move(the_explosions[i].explosion_obj);
			the_explosions[i].age--;
			if (the_explosions[i].age == 0)
			    object_set_state(the_explosions[i].explosion_obj, 
						 DYING);
		}
		else if (object_get_state(the_explosions[i].explosion_obj) == DYING)
		{
			explosion_destroy(i);
			counter++;
		}
	}
}

exp_hdl_t explosion_create(int x, int y, expsize_e size, int dx, int dy)
{
	exp_hdl_t i;
	int obj_size;
	
	for (i=0; i<MAX_NOF_EXPLOSIONS; i++)
	{
		if (the_explosions[i].explosion_obj == OBJ_VOID)
		{
			switch(size)
			{
				case EXP_BIG:
					obj_size = EXP_BIG_SIZE;
					break;
				case EXP_MEDIUM:
					obj_size = EXP_MED_SIZE;
					break;
				case EXP_SMALL:
					obj_size = EXP_SMALL_SIZE;
					break;
				default:
					obj_size = 0;
			}

			the_explosions[i].explosion_obj = 
				object_create (x, y, dx, dy, obj_size);
			the_explosions[i].size = size;
			the_explosions[i].age = NEW_EXPLOSION_AGE;
			nof_explosions++;
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
	nof_explosions--;
}

