/* $Id: asteroid.c,v 1.10 2003/02/15 12:03:56 manuel Exp $
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
#include "object.h"
#include "asteroid.h"
#include "explosio.h"

/*
 * LOCAL DEFINITIONS
 */

asteroid_t the_asteroids[MAX_NOF_ASTEROIDS];

unsigned int nof_asteroids;

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
	}
	nof_asteroids=0;
}

void asteroids_move()
{
	ast_hdl_t i;
	char counter=0;

	for (i=0; i<MAX_NOF_ASTEROIDS && counter<=nof_asteroids; i++)
	{
		if (the_asteroids[i].asteroid_obj != OBJ_VOID)
		{
			object_move(the_asteroids[i].asteroid_obj);
			counter++;
		}
	}
}

ast_hdl_t asteroid_create(int x, int y, astsize_e size)
{
	ast_hdl_t i;

	for (i=0; i<MAX_NOF_ASTEROIDS; i++)
	{
		if (the_asteroids[i].asteroid_obj == OBJ_VOID)
		{
			the_asteroids[i].asteroid_obj = 
				object_create (x, y, 0, 0, OBJ_AST);
			the_asteroids[i].size = size;
			nof_asteroids++;
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
		the_new_ast = asteroid_create (x-(dy<<1), y+(dx<<1), size - 1);
		object_accel(the_asteroids[the_new_ast].asteroid_obj,
			     (dx-dy)>>1, (dx+dy)>>1);
		the_new_ast = asteroid_create (x+(dy<<1), y-(dx<<1), size - 1);
		object_accel(the_asteroids[the_new_ast].asteroid_obj,
			     (dx+dy)>>1, (dy-dx)>>1);
	}
	
	object_destroy(&(the_asteroids[ast].asteroid_obj));
	the_asteroids[ast].size = AST_NONE;
	nof_asteroids--;

	// explode! HAHAHAHAAAAA!
	
	explosion_create(x, y, (expsize_e) size, dx, dy);
}

