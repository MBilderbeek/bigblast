/* $Id: explosio.h,v 1.2 2003/02/14 17:52:28 eric Exp $
 *
 * AUTHOR(S)   : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains typedefs of the explosion. That's all folks!
 */

#ifndef EXPLOSION_H
#define EXPLOSION_H

/*
 * INCLUDES
 */

#include "object.h"

/*
 * DEFINITIONS
 */

#define MAX_NOF_EXPLOSIONS 13 
#define NEW_EXPLOSION_AGE 8

typedef char exp_hdl_t;
extern char nof_explosions;

/*
 * TYPE DEFINITIONS
 */

typedef enum
{
	EXP_NONE,
	EXP_SMALL,
	EXP_MEDIUM,
	EXP_BIG
} expsize_e;

typedef struct
{
	obj_hdl_t explosion_obj;
	expsize_e size;
	unsigned char age;
} explosion_t;
	
extern explosion_t the_explosions[MAX_NOF_EXPLOSIONS];

//extern unsigned int nof_explosions;

/*
 * EXTERNAL FUNCTIONS
 */

extern void explosions_init();

extern void explosions_move();

extern exp_hdl_t explosion_create(int x, int y, expsize_e size, int dx, int dy);

extern void explosion_destroy(exp_hdl_t exp);

#endif
