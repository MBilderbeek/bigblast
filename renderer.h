/* $Id: renderer.h,v 1.9 2004/01/27 22:26:47 manuel Exp $
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
#define EXP_TILE_SIZE 16
#define BULLET_TILE_SIZE 3
#define SHIELD_TILE_SIZE 16

#define GFXPAGE 2

extern unsigned int frame_counter;

void render_init();
void dedbuffer();
void menuscreen_init();
void playscreen_init();
void render_frame(onoff_t boost, onoff_t shield, char noflives);
void render_info(char noflives);

#endif

