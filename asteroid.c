/* $Id: asteroid.c,v 1.2 2002/09/27 17:27:26 manuel Exp $
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

/*
 * LOCAL DEFINITIONS
 */

asteroid_t the_asteroids[MAX_NOF_ASTEROIDS];


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
}

void asteroids_move()
{
	ast_hdl_t i;

	for (i=0; i<MAX_NOF_ASTEROIDS; i++)
	{
		if (the_asteroids[i].asteroid_obj != OBJ_VOID)
			object_move(the_asteroids[i].asteroid_obj);
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
	astsize_e size = the_asteroids[ast].size;
	
	if (size > AST_SMALL)
	{
		/* create 2 new ones */
		the_new_ast = asteroid_create (x, y, size - 1);
		object_accel(the_asteroids[the_new_ast].asteroid_obj,
			     rand()%(OBJ_MAX_DXY<<1)-(OBJ_MAX_DXY), 
		             rand()%(OBJ_MAX_DXY<<1)-(OBJ_MAX_DXY));
		the_new_ast = asteroid_create (x, y, size - 1);
		object_accel(the_asteroids[the_new_ast].asteroid_obj,
			     rand()%(OBJ_MAX_DXY<<1)-(OBJ_MAX_DXY), 
		             rand()%(OBJ_MAX_DXY<<1)-(OBJ_MAX_DXY));
	}
	
	object_destroy(&(the_asteroids[ast].asteroid_obj));
	the_asteroids[ast].size = AST_NONE;
}

