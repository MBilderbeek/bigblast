/* $Id: ufo.c,v 1.1 2003/03/01 22:56:57 eric Exp $
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

/*
 * LOCAL DEFINITIONS
 */

#define UFO_SIZE (16 << OBJ_GFX_FACTOR)


ufo_t the_ufo;

/*
 * EXTERNAL FUNCTIONS
 */

void ufo_init( )
{
	the_ufo.ufo_obj = OBJ_VOID;
}

void ufo_move( )
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
			object_accel(the_ufo.ufo_obj,
					heading2dxdy[heading].x << 2,
					heading2dxdy[heading].y << 2);
			counter = 5 + (rand() % 20);
		}
		else counter--;
		object_move(the_ufo.ufo_obj);
	}
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

