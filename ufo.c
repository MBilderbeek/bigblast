/* $Id: ufo.c,v 1.2 2003/04/03 23:07:23 manuel Exp $
 * 
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to handle the ufo. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include "stdlib.h"
#include "object.h"
#include "ship.h"
#include "ufo.h"
#include "explosio.h"
#include "bullet.h"

/*
 * LOCAL DEFINITIONS
 */

#define UFO_SIZE (16 << OBJ_GFX_FACTOR)


ufo_t the_ufo;

bullet_t the_ufo_bullet;

/*
 * EXTERNAL FUNCTIONS
 */

void ufo_init( )
{
	the_ufo.ufo_obj = OBJ_VOID;
	the_ufo_bullet.age = 0;
	the_ufo_bullet.bullet_obj = OBJ_VOID;
}

void ufo_fire()
{
	if (the_ufo_bullet.bullet_obj == OBJ_VOID)
	{
		obj_hdl_t ship_obj = the_ship.ship_obj;
		obj_hdl_t ufo_obj = the_ufo.ufo_obj;
		// obj2gfx om 't wat in te binden qua getallen betreft
		int delta_x = OBJ2GFX(object_get_x(ship_obj) - object_get_x(ufo_obj));
		int delta_y = OBJ2GFX(object_get_y(ship_obj) - object_get_y(ufo_obj));
		
		the_ufo_bullet.bullet_obj = 
				object_create (object_get_x(ufo_obj)+(UFO_SIZE>>1), 
								object_get_y(ufo_obj)+(UFO_SIZE>>1), 
								delta_x, delta_y, BULLET_SIZE);
		the_ufo_bullet.age = NEW_BULLET_AGE;	
	}
}	


void ufo_move()
{
	static char counter=0;
	int dx, dy;
	int heading;

	if (the_ufo.ufo_obj != OBJ_VOID)
	{
		if (counter == 0)
		{
			dx = object_get_dx(the_ufo.ufo_obj);
			dy = object_get_dy(the_ufo.ufo_obj);
			object_accel(the_ufo.ufo_obj, -dx, -dy);
			heading = rand()%MAX_HEADING;
			object_accel(the_ufo.ufo_obj, heading2dxdy[heading].x << 2,
					heading2dxdy[heading].y << 2);
			ufo_fire();
			counter = 5 + (rand() % 20);
		}
		else counter--;
		object_move(the_ufo.ufo_obj);
	}
}

void ufo_bullet_move()
{
	if (the_ufo_bullet.age > 0)
	{
		object_move(the_ufo_bullet.bullet_obj);
		the_ufo_bullet.age--;
		if (the_ufo_bullet.age == 0)
				object_set_state(the_ufo_bullet.bullet_obj, DYING);
	}
	else if (object_get_state(the_ufo_bullet.bullet_obj) == DYING)
			object_destroy(&(the_ufo_bullet.bullet_obj));
}

void ufo_create( int x, int y )
{
	if (the_ufo.ufo_obj == OBJ_VOID)
	{
		the_ufo.ufo_obj = object_create (x, y, 0, 0, UFO_SIZE);
	}
}

void ufo_destroy( char explode )
{
	int x = object_get_x(the_ufo.ufo_obj);
	int y = object_get_y(the_ufo.ufo_obj);
	int dx = object_get_dx(the_ufo.ufo_obj);
	int dy = object_get_dy(the_ufo.ufo_obj);

	object_destroy(&(the_ufo.ufo_obj));
	if (explode)
		explosion_create(x, y, EXP_BIG, dx, dy);
}

