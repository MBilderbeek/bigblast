/* $Id: ufo.h,v 1.1 2003/03/01 22:56:57 eric Exp $
 *
 * AUTHOR(S)   : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains typedefs of the ufo. That's all folks!
 */

#ifndef UFO_H
#define UFO_H

/*
 * INCLUDES
 */

#include "object.h"

/*
 * DEFINITIONS
 */

/*
 * TYPE DEFINITIONS
 */

typedef struct
{
	obj_hdl_t ufo_obj;
} ufo_t;
	
extern ufo_t the_ufo;

/*
 * EXTERNAL FUNCTIONS
 */

extern void ufo_init();

extern void ufo_move();

extern void ufo_create(int x, int y );

extern void ufo_destroy(char explode);

#endif
