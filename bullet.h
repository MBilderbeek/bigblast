/* $Id: bullet.h,v 1.4 2003/04/03 23:07:23 manuel Exp $
 *
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

#define MAX_NOF_BULLETS 6
#define NEW_BULLET_AGE   12
#define BULLET_SIZE      ((int)2 << OBJ_GFX_FACTOR)

/*
 * TYPE DEFINITIONS
 */

typedef struct
{
	obj_hdl_t bullet_obj;
	unsigned char age;
} bullet_t;
	
extern bullet_t the_bullets[MAX_NOF_BULLETS];
extern int nof_bullets;

/*
 * EXTERNAL FUNCTIONS
 */

extern void bullets_init();

extern void bullets_move();

extern void bullet_fire();

#endif
