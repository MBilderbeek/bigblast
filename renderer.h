/* $Id: renderer.h,v 1.10 2004/08/22 20:01:27 eric Exp $
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

#include <glib.h>
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
void menuscreen_init();
void playscreen_init();
void render_frame(onoff_t boost, onoff_t shield, char noflives);
void render_info(char noflives);

// double buffering
#define db_start() setpg(0,1)
#define db_stop()  setpg(0,0)
#define db_swap()  setpg(c_apage,c_dpage)

#endif

