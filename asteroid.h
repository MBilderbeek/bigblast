/* $Id: asteroid.h,v 1.3 2002/10/17 22:09:23 manuel Exp $
 *
 * AUTHOR(S)   : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains typedefs of the asteroid. That's all folks!
 */

#ifndef ASTEROID_H
#define ASTEROID_H

/*
 * INCLUDES
 */

#include "object.h"

/*
 * DEFINITIONS
 */

#define MAX_NOF_ASTEROIDS 20

typedef char ast_hdl_t;

/*
 * TYPE DEFINITIONS
 */

typedef enum
{
	AST_NONE,
	AST_SMALL,
	AST_MEDIUM,
	AST_BIG
} astsize_e;

typedef struct
{
	obj_hdl_t asteroid_obj;
	astsize_e size;
} asteroid_t;
	
extern asteroid_t the_asteroids[MAX_NOF_ASTEROIDS];

extern unsigned int nof_asteroids;

/*
 * EXTERNAL FUNCTIONS
 */

extern void asteroids_init();

extern void asteroids_move();

extern ast_hdl_t asteroid_create(int x, int y, astsize_e size);

extern void asteroid_destroy(ast_hdl_t ast);

#endif
