/* $Id: collisio.c,v 1.8 2003/01/12 23:03:56 manuel Exp $
 *
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to detect collisions. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include "glib.h"
#include "object.h"
#include "ship.h"
#include "asteroid.h"
#include "bullet.h"
#include "renderer.h"
#include "scores.h"

/*
 * LOCAL DEFINITIONS
 */

#define AST_BIG_DIA (AST_TILE_SIZE - 2)
#define AST_MEDIUM_DIA (AST_TILE_SIZE - 4)
#define AST_SMALL_DIA (AST_TILE_SIZE - 8)

#define SHIP_DIA (SHIP_TILE_SIZE - 4)

/*
 * EXTERNAL FUNCTIONS
 */

static int iabs(int i)
{
	return (i > 0)? i: -i;
}

static int get_ast_dia(ast_hdl_t i)
{
	int ast_dia;

	switch (the_asteroids[i].size)
	{
		case AST_BIG:
			ast_dia = GFX2OBJ(AST_BIG_DIA);
			break;
	 	case AST_MEDIUM:
			ast_dia = GFX2OBJ(AST_MEDIUM_DIA);
			break;
	 	case AST_SMALL:
			ast_dia = GFX2OBJ(AST_SMALL_DIA);
			break;
		default:
			break;
	}

	return ast_dia;
}

char ship_hit(onoff_t shield)
{
	char i, hit=0;
	int ship_x = object_get_x(the_ship.ship_obj); 
	int ship_y = object_get_y(the_ship.ship_obj); 
	char ship_dx = object_get_dx(the_ship.ship_obj); 
	char ship_dy = object_get_dy(the_ship.ship_obj);
	obj_hdl_t ast_obj;
	astsize_e ast_size;
	int ast_x, ast_y;
	int delta_x, delta_y, delta_dia;

	ship_x += GFX2OBJ(SHIP_TILE_SIZE) >> 1;
	ship_y += GFX2OBJ(SHIP_TILE_SIZE) >> 1;

	for (i=0; i<MAX_NOF_ASTEROIDS && !hit; i++)
	{
		if (the_asteroids[i].size != AST_NONE)
		{
			ast_obj = the_asteroids[i].asteroid_obj;
			delta_dia = (get_ast_dia(i) + GFX2OBJ(SHIP_DIA)) >> 1;
			ast_x = object_get_x(ast_obj) + ( GFX2OBJ(AST_TILE_SIZE) >> 1 );
			delta_x = ast_x - ship_x;
			if (iabs(delta_x) <= delta_dia)
			{
				ast_y = object_get_y(ast_obj) + ( GFX2OBJ(AST_TILE_SIZE) >> 1 );
				delta_y = ast_y - ship_y;
				if (iabs(delta_y) <= delta_dia)
				{
					/* calc speed of asteroid with respect to ship (ast's momentum) */
					delta_x = object_get_dx(ast_obj) - ship_dx;
					delta_y = object_get_dy(ast_obj) - ship_dy;
					ast_size = the_asteroids[i].size;

					/* ship gets (part of) ast's momentum */
					object_accel(the_ship.ship_obj,
						     delta_x >> (3 - ast_size), delta_y >> (3 - ast_size));

					/* ast loses (part of) its momentum */
					object_accel(ast_obj,
						     -(delta_x >> ast_size), -(delta_y >> ast_size));

					ship_dx = object_get_dx(the_ship.ship_obj); 
					ship_dy = object_get_dy(the_ship.ship_obj); 
					
					object_set_state(ast_obj, DYING);
					if (!shield) hit = 1;
				}
			}
		}

	}
	if (hit && !shield){
		object_set_state(the_ship.ship_obj,DYING);
	}
	return (hit);
}


void bullets_n_asteroids()
{
	int i, b;
	int ax, ay;
	int bx, by;
	int dx, dy;
	int dia;

	state_e ast_state;
	
	for (i=0; i<MAX_NOF_ASTEROIDS; i++)		
	{
		if(the_asteroids[i].size != AST_NONE)
		{
			ast_state = 
			    object_get_state(the_asteroids[i].asteroid_obj);

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
				ax =
				   object_get_x(the_asteroids[i].asteroid_obj)
				   + ( GFX2OBJ(AST_TILE_SIZE) >> 1 );
				ay =
				   object_get_y(the_asteroids[i].asteroid_obj)
				   + ( GFX2OBJ(AST_TILE_SIZE) >> 1 );
				dia = get_ast_dia(i) >> 1;
	
				for (b=0; b<MAX_NOF_BULLETS; b++)
				{
					if(the_bullets[b].age > 1)
					{
						bx = object_get_x(the_bullets[b].bullet_obj);
						dx = ax - bx;
						if (iabs(dx) <= dia)
						{
							by = object_get_y(the_bullets[b].bullet_obj);
							dy = ay - by;
							if (iabs(dy) <= dia)
							{
								object_set_state(the_asteroids[i].asteroid_obj, DYING);
								the_bullets[b].age = 1;
							}
						}
					}
				}
				break;
			default: break;
			}
		}
	}
}

