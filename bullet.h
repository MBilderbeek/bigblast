/* 
 * FILE        : bullet.h 
 * LAST UPDATE : 09 Jun 2002
 * AUTHOR(S)   : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains typedefs of the bullet. That's all folks!
 */

#ifndef BULLET_H
#define BULLET_H

/*
 * INCLUDES
 */

#include "object.h"

/*
 * DEFINITIONS
 */

#define MAX_NOF_BULLETS 8

/*
 * TYPE DEFINITIONS
 */

typedef struct
{
	obj_hdl_t bullet_obj;
	unsigned char age;
} bullet_t;
	
extern bullet_t the_bullets[MAX_NOF_BULLETS];

/*
 * EXTERNAL FUNCTIONS
 */

extern void bullets_init();

extern void bullets_move();

extern void bullet_fire();

#endif
