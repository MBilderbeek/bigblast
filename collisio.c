/* $Id: collisio.c,v 1.19 2003/05/29 21:56:46 manuel Exp $
 *
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to detect collisions. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include <stdio.h>
#include "glib.h"
#include "object.h"
#include "ship.h"
#include "asteroid.h"
#include "ufo.h"
#include "bullet.h"
#include "scores.h"

/*
 * LOCAL DEFINITIONS
 */

//#define AST_SHIP_SLACK GFX2OBJ(4) -> move to ship size

/*
 * EXTERNAL FUNCTIONS
 */

static int iabs(int i)
{
	return (i > 0)? i: -i;
}

static int get_center_x(obj_hdl_t obj)
{
	return object_get_x(obj) + (object_get_size(obj) >> 1);
}

static int get_center_y(obj_hdl_t obj)
{
	return object_get_y(obj) + (object_get_size(obj) >> 1);
}

static char objects_hit(obj_hdl_t obj1, obj_hdl_t obj2) // object_shit?
{
	int x1 = get_center_x(obj1);
	int dist = (object_get_size(obj1)+object_get_size(obj2)) >> 1; //=dia1+dia2
	int x2 = get_center_x(obj2);
	char result=0;

	if (iabs(x2-x1) <= dist)
	{
		int y1 = get_center_y(obj1);
		int y2 = get_center_y(obj2);
		if (iabs(y2-y1) <= dist) result=1;
	}
	return result;
}

char ship_hit(void) // hip shit?
{
	char      i, hit   = 0;
	obj_hdl_t ship_obj = the_ship.ship_obj;
	obj_hdl_t ast_obj, ufo_obj, ufo_bullet_obj;
	int       weight;
	int       delta_dx, delta_dy;
	char      counter = 0;

	// check asteroids
	for (i=0; i<MAX_NOF_ASTEROIDS && !hit && counter<=nof_asteroids; i++)
	{
		if (the_asteroids[i].size != AST_NONE)
		{
			counter++;
			ast_obj = the_asteroids[i].asteroid_obj;

			if (objects_hit(ast_obj, ship_obj))
			{
				/* calc speed of asteroid with respect
				 * to ship (ast's momentum) */
				delta_dx = object_get_dx(ast_obj) - object_get_dx(ship_obj);
				delta_dy = object_get_dy(ast_obj) - object_get_dy(ship_obj);

				/* In case we don't move, make sure we do */
				if (delta_dx==0 && delta_dy==0)
				{
					delta_dx=OBJ2GFX(get_center_x(ship_obj)-get_center_x(ast_obj));
					delta_dy=OBJ2GFX(get_center_y(ship_obj)-get_center_y(ast_obj));
				}
					
				weight = AST_BIG - the_asteroids[i].size;

				/* ship gets (part of) ast's momentum */
				object_accel(ship_obj, delta_dx >> weight, delta_dy >> weight);

				if (the_asteroids[i].type==AST_TYPE_NORMAL)
				    object_set_state(ast_obj, DYING); //destroy in bullets check
				else
					object_accel(ast_obj, 
									-(delta_dx>>weight),-(delta_dy>>weight));
					
				hit = 1;
			}
		}
	}
	
	// check ufo
	if (((ufo_obj = the_ufo.ufo_obj) != OBJ_VOID) && 
			objects_hit(ufo_obj, ship_obj))
	{
		delta_dx = object_get_dx(ufo_obj) - object_get_dx(ship_obj);
		delta_dy = object_get_dy(ufo_obj) - object_get_dy(ship_obj);

		object_accel(ship_obj, delta_dx, delta_dy);
		
		object_set_state(ufo_obj, DYING);
		hit=1;
	}
	// check ufo bullet
	if (((ufo_bullet_obj = the_ufo_bullet.bullet_obj) != OBJ_VOID) &&
				objects_hit(ufo_bullet_obj, ship_obj)) 
	{
			the_ufo_bullet.age = 1; // kaput!
			hit = 1;
	}
	
	return (hit);
}

static void bullets_n_object(obj_hdl_t obj, char unbreakable)
{
	int b;
	obj_hdl_t bul_obj;

	for (b=0; b<MAX_NOF_BULLETS; b++)
	{
		if(the_bullets[b].age > 1)
		{
			bul_obj = the_bullets[b].bullet_obj;
			if (objects_hit(bul_obj, obj))
			{
				if (unbreakable)
				{
					int bx = object_get_dx(bul_obj);
					int by = object_get_dy(bul_obj);
					object_accel(obj, (bx>>5),
							(by>>5));
				}
				else
					object_set_state(obj, DYING);
				the_bullets[b].age = 1;
			}
		}
	}
}

void bullets_n_ufo()
{
	obj_hdl_t ufo_obj = the_ufo.ufo_obj;
	
	if(ufo_obj != OBJ_VOID)
	{
		state_e ufo_state = object_get_state(ufo_obj);

		switch(ufo_state)
		{
			case DYING:
				score+=SC_UFO; 
				ufo_destroy(1); // with explosion
				break;
			case ALIVE:
			case NEW:
				bullets_n_object(ufo_obj, 0); // not steel
				break;
			default: break;
		}
	}
}

void bullets_n_asteroids()
{
	int i;
	char counter=0;

	obj_hdl_t ast_obj;
	state_e ast_state;
	
	for (i=0; i<MAX_NOF_ASTEROIDS && counter<=nof_asteroids; i++)		
	{
		if(the_asteroids[i].size != AST_NONE)
		{
			counter++;
			ast_obj = the_asteroids[i].asteroid_obj;
			ast_state = object_get_state(ast_obj);

			switch(ast_state)
			{
			case DYING:
				switch (the_asteroids[i].size)
				{
					case AST_SMALL: score+=SC_AST_SMALL; break;
					case AST_MEDIUM: score+=SC_AST_MEDIUM; break;
					case AST_BIG: score+=SC_AST_BIG; break;
					default: ;
				}
				asteroid_destroy(i);
				break;
			case ALIVE:
			case NEW:
				bullets_n_object(the_asteroids[i].asteroid_obj,
						 the_asteroids[i].type!=AST_TYPE_NORMAL);
				break;
			default: break;
			}
		}
	}
}


