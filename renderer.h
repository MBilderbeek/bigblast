/* $Id: renderer.h,v 1.5 2002/11/19 23:18:15 manuel Exp $
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
#define SHIELD_TILE_SIZE 16
#define OBJ2GFX(coord) ((coord) >> 4)
#define GFX2OBJ(coord) ((coord) << 4)

extern unsigned int frame_counter;

void render_init();
void playscreen_init();
void render_frame(onoff_t boost, onoff_t shield, char noflives);
void write(char *string, unsigned int x, unsigned int  y);
void write_cent(char *string, unsigned int  y);

#endif

