/* $Id: bullet.c,v 1.6 2003/04/03 23:07:23 manuel Exp $
 *
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to handle the bullets. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include "object.h"
#include "ship.h" // defines the heading2dxdy table
#include "bullet.h"

/*
 * LOCAL DEFINITIONS
 */

#define BULLET_SPEED(x)  ((x) << 2)

bullet_t the_bullets[MAX_NOF_BULLETS];
int nof_bullets;

/*
 * EXTERNAL FUNCTIONS
 */

void bullets_init()
{
	char i;
	
	for (i=0; i<MAX_NOF_BULLETS; i++)
	{
		the_bullets[i].age = 0;
		the_bullets[i].bullet_obj = OBJ_VOID;
		nof_bullets=0;
	}
}

void bullets_move()
{
	char i;

	for (i=0; i<MAX_NOF_BULLETS; i++)
	{
		if (the_bullets[i].age > 0)
		{
			object_move(the_bullets[i].bullet_obj);
			the_bullets[i].age--;
			if (the_bullets[i].age == 0)
				object_set_state(the_bullets[i].bullet_obj, 
						 DYING);
		}
		else if (object_get_state(the_bullets[i].bullet_obj) == DYING)
		{
			object_destroy(&(the_bullets[i].bullet_obj));
			nof_bullets--;
		}
	}
}

void bullet_fire()
{
	char i;
	char heading = the_ship.heading;
	char dx = BULLET_SPEED(heading2dxdy[heading].x) ;
	char dy = BULLET_SPEED(heading2dxdy[heading].y) ;
	obj_hdl_t ship_obj = the_ship.ship_obj;
	int x = object_get_x(ship_obj) + (object_get_size(ship_obj)>>1) + dx ;
	int y = object_get_y(ship_obj) + (object_get_size(ship_obj)>>1) + dy ;
	
	for(i=0; i < MAX_NOF_BULLETS; i++)
	{
		if (the_bullets[i].bullet_obj == OBJ_VOID)
		{
			the_bullets[i].bullet_obj = 
				object_create (x, y, dx, dy, BULLET_SIZE);
			the_bullets[i].age = NEW_BULLET_AGE;
			nof_bullets++;
			object_accel(the_ship.ship_obj, -(dx>>5), -(dy>>5));
			i = MAX_NOF_BULLETS; /* get out of the for loop */
		}
	}
}

