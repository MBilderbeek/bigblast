/* $Id: renderer.h,v 1.2 2002/09/27 17:27:26 manuel Exp $
 *
 * AUTHOR(S)   : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   interface definition for the renderer.
 */

#ifndef RENDERER_H
#define RENDERER_H

/*
 * INCLUDES
 */

#include "ship.h"

/*
 * DEFINITIONS
 */

#define SHIP_TILE_SIZE 16
#define AST_TILE_SIZE 16
#define BULLET_TILE_SIZE 3
#define OBJ2GFX(coord) ((coord) >> 4)
#define GFX2OBJ(coord) ((coord) << 4)

void render_init();
void render_frame(onoff_t boost);

#endif

