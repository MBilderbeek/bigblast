/* $Id: bullet.c,v 1.3 2003/02/07 01:38:46 manuel Exp $
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
#include "renderer.h"
#include "bullet.h"

/*
 * LOCAL DEFINITIONS
 */

#define NEW_BULLET_AGE 12
#define BULLET_SPEED(x) ((x) * 7)

bullet_t the_bullets[MAX_NOF_BULLETS];

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
			object_destroy(&(the_bullets[i].bullet_obj));
	}
}

void bullet_fire()
{
	char i;
	char heading = the_ship.heading;
	char dx = BULLET_SPEED(heading2dxdy[heading].x) ;
	char dy = BULLET_SPEED(heading2dxdy[heading].y) ;
	int x = object_get_x(the_ship.ship_obj) + dx + dx
					 + GFX2OBJ( (SHIP_TILE_SIZE-3) >> 1 );
	int y = object_get_y(the_ship.ship_obj) + dy + dy
					 + GFX2OBJ( (SHIP_TILE_SIZE-3) >> 1 );
	
	for(i=0; i < MAX_NOF_BULLETS; i++)
	{
		if (the_bullets[i].bullet_obj == OBJ_VOID)
		{
			the_bullets[i].bullet_obj = 
				object_create (x, y, dx, dy, OBJ_BULLET);
			the_bullets[i].age = NEW_BULLET_AGE;
			object_accel(the_ship.ship_obj, -(dx>>5), -(dy>>5));
			i = MAX_NOF_BULLETS; /* get out of the for loop */
		}
	}
}

