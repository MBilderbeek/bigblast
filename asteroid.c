/* $Id: asteroid.c,v 1.14 2003/05/29 21:56:46 manuel Exp $
 * 
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to handle the asteroids. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include "stdlib.h"
#include "math.h"
#include "object.h"
#include "asteroid.h"
#include "explosio.h"
#include "ship.h"

/*
 * LOCAL DEFINITIONS
 */

#define AST_BIG_SIZE  (16 << OBJ_GFX_FACTOR)
#define AST_MED_SIZE  (11 << OBJ_GFX_FACTOR)
#define AST_SMALL_SIZE (7 << OBJ_GFX_FACTOR)

asteroid_t the_asteroids[MAX_NOF_ASTEROIDS];

unsigned int nof_asteroids; // totaal
unsigned int nof_steel_asteroids;

/*
 * EXTERNAL FUNCTIONS
 */

void asteroids_init()
{
	ast_hdl_t i;
	
	for (i=0; i<MAX_NOF_ASTEROIDS; i++)
	{
		the_asteroids[i].size = AST_NONE;
		the_asteroids[i].asteroid_obj = OBJ_VOID;
		the_asteroids[i].type = AST_TYPE_NORMAL;
	}
	nof_asteroids=0;
}

void asteroids_move()
{
	ast_hdl_t i;
	char counter=0;
	static char homingsync=0;
			
	homingsync=(homingsync+1)&3;

	for (i=0; i<MAX_NOF_ASTEROIDS && counter<=nof_asteroids; i++)
	{
		obj_hdl_t ast_obj=the_asteroids[i].asteroid_obj;

		if (ast_obj != OBJ_VOID)
		{
			if (the_asteroids[i].type==AST_TYPE_HOMING && !homingsync)
			{
				obj_hdl_t ship_obj = the_ship.ship_obj;
				// obj2gfx om 't wat in te binden qua getallen betreft
				int delta_x = OBJ2GFX(object_get_x(ship_obj) - object_get_x(ast_obj));
				int delta_y = OBJ2GFX(object_get_y(ship_obj) - object_get_y(ast_obj));
				int l=mysqrt((delta_x*delta_x)+(delta_y*delta_y));
				int angle=0;
				
				// normalise
				delta_x/=(l>>4);
				delta_y/=(l>>4);

				// inverse (co)sine table lookup
				for (angle=0; angle<16 && heading2dxdy[angle].x>delta_x; angle++)
				/*zippo*/		;
				if (delta_y<0) angle=31-angle;
				
				object_accel(ast_obj, (heading2dxdy[angle].x>>1), (heading2dxdy[angle].y)>>1);
				
			}
			object_move(the_asteroids[i].asteroid_obj);
			counter++;
		}
	}
}

ast_hdl_t asteroid_create(int x, int y, astsize_e size, asttype_e type)
{
	ast_hdl_t i;
	int obj_size;

	for (i=0; i<MAX_NOF_ASTEROIDS; i++)
	{
		if (the_asteroids[i].asteroid_obj == OBJ_VOID)
		{
			switch(size)
			{
				case AST_BIG:
					obj_size = AST_BIG_SIZE;
					break;
				case AST_MEDIUM:
					obj_size = AST_MED_SIZE;
					break;
				case AST_SMALL:
					obj_size = AST_SMALL_SIZE;
					break;
				default:
					obj_size = 0;
			}
			the_asteroids[i].asteroid_obj = 
				object_create (x, y, 0, 0, obj_size);
			the_asteroids[i].size = size;
			the_asteroids[i].type = type;
			nof_asteroids++;
			if (type!=AST_TYPE_NORMAL) nof_steel_asteroids++;
			break;
		}
	}

	/* gaat altijd goed (waaaah) */

	return i;
}

void asteroid_destroy(ast_hdl_t ast)
{
	ast_hdl_t the_new_ast;
	int x = object_get_x(the_asteroids[ast].asteroid_obj);
	int y = object_get_y(the_asteroids[ast].asteroid_obj);
	int dx = object_get_dx(the_asteroids[ast].asteroid_obj);
	int dy = object_get_dy(the_asteroids[ast].asteroid_obj);
	astsize_e size = the_asteroids[ast].size;
	
	if (size > AST_SMALL)
	{
		/* create 2 new ones */
		the_new_ast = asteroid_create (x-(dy<<1), y+(dx<<1), size-1,0);
		object_accel(the_asteroids[the_new_ast].asteroid_obj,
			     (dx-dy)>>1, (dx+dy)>>1);
		the_new_ast = asteroid_create (x+(dy<<1), y-(dx<<1), size-1,0);
		object_accel(the_asteroids[the_new_ast].asteroid_obj,
			     (dx+dy)>>1, (dy-dx)>>1);
	}
	
	object_destroy(&(the_asteroids[ast].asteroid_obj));
	the_asteroids[ast].size = AST_NONE;
	nof_asteroids--;

	// explode! HAHAHAHAAAAA!
	
	explosion_create(x, y, (expsize_e) size, dx, dy);
}

void kill_all_asteroids()
{
	ast_hdl_t i;

	for (i=0; i<MAX_NOF_ASTEROIDS; i++)
	{
		if (the_asteroids[i].asteroid_obj != OBJ_VOID)
		{
			object_destroy(&(the_asteroids[i].asteroid_obj));
		}
	}

	nof_asteroids=nof_steel_asteroids=0; // opgeruimd staat netjes!
}
