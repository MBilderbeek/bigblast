/* $Id: collisio.c,v 1.2 2002/09/27 17:27:26 manuel Exp $
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

/*
 * LOCAL DEFINITIONS
 */

#define ABS(x) ((x)<0 ? -(x) : (x))

#define AST_BIG_DIA (AST_TILE_SIZE - 2)
#define AST_MEDIUM_DIA (AST_TILE_SIZE - 4)
#define AST_SMALL_DIA (AST_TILE_SIZE - 8)

#define SHIP_DIA (SHIP_TILE_SIZE - 4)

/*
 * EXTERNAL FUNCTIONS
 */

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

char ship_hit(obj_hdl_t *object)
{
	char i, hit=0;
	int ship_x = object_get_x(the_ship.ship_obj); 
	int ship_y = object_get_y(the_ship.ship_obj); 
	int ast_x, ast_y;
	int delta_x, delta_y, delta_dia;

	ship_x += GFX2OBJ(SHIP_TILE_SIZE) >> 1; //kutcompiler
	ship_y += GFX2OBJ(SHIP_TILE_SIZE) >> 1;

	for (i=0; i<MAX_NOF_ASTEROIDS && !hit; i++)
	{
		if (the_asteroids[i].size != AST_NONE)
		{
			delta_dia = (get_ast_dia(i) + GFX2OBJ(SHIP_DIA)) >> 1;
			ast_x = object_get_x(the_asteroids[i].asteroid_obj) + 
				( GFX2OBJ(AST_TILE_SIZE) >> 1 );
			delta_x = ast_x - ship_x;
			delta_x = ABS(delta_x); // we can optimize this!
			if (delta_x <= delta_dia)
			{
				ast_y = object_get_y(the_asteroids[i].asteroid_obj) + 
					( GFX2OBJ(AST_TILE_SIZE) >> 1 );
				delta_y = ast_y - ship_y;
				delta_y = ABS(delta_y); // see above
			
				if (delta_y <= delta_dia)
					hit = 1;
			}
		}

	}
	*object = the_asteroids[--i].asteroid_obj; 
	if (hit) object_set_state(the_ship.ship_obj,DYING); // side effect
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
	
				/* for all bullets */
				for (b=0; b<MAX_NOF_BULLETS; b++)
				{
					if(the_bullets[b].age > 1)
					{
						bx = object_get_x(the_bullets[b].bullet_obj);
						dx = ax - bx;
						dx = ABS(dx);
						if (dx <= dia)
						{
							by = object_get_y(the_bullets[b].bullet_obj);
							dy = ay - by;
							dy = ABS(dy);
							if (dy <= dia)
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

