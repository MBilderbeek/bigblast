/* $Id: collisio.c,v 1.12 2003/02/28 00:24:34 manuel Exp $
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
#include "scores.h"

/*
 * LOCAL DEFINITIONS
 */

#define AST_SHIP_SLACK GFX2OBJ(4)

static void bullets_n_one_ast(asteroid_t *ast);
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

char ship_hit(void)
{
	char      i, hit   = 0;
	obj_hdl_t ship_obj = the_ship.ship_obj;
	int       ship_x   = get_center_x(ship_obj); 
	int       ship_y   = get_center_y(ship_obj); 
	char      ship_dx;
	char      ship_dy;
	obj_hdl_t ast_obj;
	astsize_e ast_size;
	int       ast_x, ast_y;
	int       delta_x, delta_y, delta_dia;
	char      counter = 0;

	for (i=0; i<MAX_NOF_ASTEROIDS && !hit && counter<=nof_asteroids; i++)
	{
		if (the_asteroids[i].size != AST_NONE)
		{
			counter++;
			ast_obj = the_asteroids[i].asteroid_obj;

			delta_dia = object_get_size(ast_obj);
			ast_x = get_center_x(ast_obj);
			
			delta_dia += object_get_size(ship_obj)-AST_SHIP_SLACK;
			delta_dia >>= 1;
			
			delta_x = ast_x - ship_x;
			if (iabs(delta_x) <= delta_dia)
			{
				ast_y = get_center_y(ast_obj);
				delta_y = ast_y - ship_y;
				if (iabs(delta_y) <= delta_dia)
				{
					ship_dx = object_get_dx(ship_obj); 
					ship_dy = object_get_dy(ship_obj); 
					/* calc speed of asteroid with respect
					 * to ship (ast's momentum) */
					delta_x = object_get_dx(ast_obj)
						  - ship_dx;
					delta_y = object_get_dy(ast_obj)
						  - ship_dy;
					ast_size = the_asteroids[i].size;

					/* ship gets (part of) ast's momentum */
					object_accel(the_ship.ship_obj,
						   delta_x >> (3 - ast_size),
						   delta_y >> (3 - ast_size));

					/* ast loses (part of) its momentum */
					// But this looks unnatural since it is
					// only debris!
					//object_accel(ast_obj,
					//	     -(delta_x >> ast_size),
					//	     -(delta_y >> ast_size));

					
					if (!the_asteroids[i].steel)
					    object_set_state(ast_obj, DYING);
					else
						object_accel(ast_obj,
						    -(delta_x >> (4-ast_size)),
						    -(delta_y >> (4-ast_size)));
						
					hit = 1;
				}
			}
		}
	}
	return (hit);
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
				bullets_n_one_ast(&the_asteroids[i]);
				break;
			default: break;
			}
		}
	}
}

static void bullets_n_one_ast(asteroid_t *ast)
{
	obj_hdl_t ast_obj = ast->asteroid_obj;
	int ax = get_center_x(ast_obj);
	int ay = get_center_y(ast_obj);
	int ast_dia = object_get_size(ast_obj) >> 1;

	int b, bx, by, bul_dia;

	obj_hdl_t bul_obj;

	for (b=0; b<MAX_NOF_BULLETS; b++)
	{
		if(the_bullets[b].age > 1)
		{
			bul_obj = the_bullets[b].bullet_obj;
			bul_dia = object_get_size(bul_obj) >> 1;
			bx = get_center_x(bul_obj);
			if (iabs(ax-bx) <= ast_dia + bul_dia)
			{
				by = get_center_y(bul_obj);
				if (iabs(ay-by) <= ast_dia + bul_dia)
				{
					if (ast->steel)
					{
						bx = object_get_dx(bul_obj);
						by = object_get_dy(bul_obj);
						object_accel(ast_obj, (bx>>5),
								(by>>5));
					}
					else
						object_set_state(ast_obj, DYING);
					the_bullets[b].age = 1;
				}
			}
		}
	}
}
	
