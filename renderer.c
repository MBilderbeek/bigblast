/*
 * FILE        : renderer.c
 * LAST UPDATE : 23 Jun 2002
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to render frames. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include "glib.h"
#include "ship.h"
#include "asteroid.h"
#include "bullet.h"
#include "renderer.h"
#include "object.h"

extern void loadgrp(char *filename, unsigned int x, unsigned char y, char page);

/*
 * LOCAL DEFINITIONS
 */

#define BOOST_OFFSET 32

#define AST_SX_BIG 0
#define AST_SX_MEDIUM (4*AST_TILE_SIZE)
#define AST_SX_SMALL (8*AST_TILE_SIZE)
#define AST_SY (4*SHIP_TILE_SIZE)

#define GFXPAGE 2

static void render_ship(onoff_t boost);
static void render_asteroids();
static void render_bullets();

int *Timer=(int *)0xFC9E;                                /*Systeemtimer*/

unsigned int frame_counter = 0;

/*
 * EXTERNAL FUNCTIONS
 */

void render_init()
{
	char filename[16];
	
	ginit();
	color(15,0,0);
	screen(5);
	strcpy(filename,"\"SHIPS.COP\""); 
	loadgrp(filename, 0, 0, GFXPAGE);
}

void render_frame(onoff_t boost)
{
	while (*Timer<5);
	*Timer=0;
	frame_counter++;
	render_ship(boost);
	render_asteroids();
	render_bullets();
#ifdef DEBUG_RENDERER
	{
		int i;
		for(i=0; i < 50; i++)
		{
			pset(i,0,object_get_type(i)+2, PSET);
		}
	}
#endif
}

static void render_ship(onoff_t boost)
{
	int sx, sy;
	int dx, dy;
	int dx_prev, dy_prev;
	
	int x_cur = object_get_x(the_ship.ship_obj);
	int y_cur = object_get_y(the_ship.ship_obj);
	int x_prev = object_get_x_prev(the_ship.ship_obj);
	int y_prev = object_get_y_prev(the_ship.ship_obj);
	
	sx = (the_ship.heading & 0x0F) << 4; 
	sy = the_ship.heading & 0xF0; 

	dx = OBJ2GFX( x_cur );
	dy = OBJ2GFX( y_cur );
	dx_prev = OBJ2GFX( x_prev );
	dy_prev = OBJ2GFX( y_prev );
	
	if (boost == ON) 
		sy+=BOOST_OFFSET;
	
	if ( (x_cur != x_prev) || (y_cur != y_prev) ||
	     (the_ship.heading != the_ship.heading_prev) ||
	     (object_get_state(the_ship.ship_obj) == NEW) )
	{
		if ( object_get_state(the_ship.ship_obj) == NEW )
			object_set_state(the_ship.ship_obj, ALIVE);
		else
			boxfill(dx_prev, dy_prev, dx_prev+SHIP_TILE_SIZE-1, 
				dy_prev+SHIP_TILE_SIZE-1, 0, PSET);
		if (object_get_state(the_ship.ship_obj) != DYING)
			cpyv2v(sx, sy, sx+SHIP_TILE_SIZE-1, sy+SHIP_TILE_SIZE-1,
				GFXPAGE, dx, dy, 0, TPSET);
	}
}
	
static void render_asteroids()
{
	char i;
	int sx = 0;
	int dx, dy;
	int dx_prev, dy_prev;
	int x_cur, y_cur;
	int x_prev, y_prev;
	char animstep = frame_counter%4;
	state_e state;
	
	for (i=0; i<MAX_NOF_ASTEROIDS; i++)
	{
		switch (the_asteroids[i].size)
		{
			case AST_BIG:    sx=AST_SX_BIG;    break;
		 	case AST_MEDIUM: sx=AST_SX_MEDIUM; break;
			case AST_SMALL:  sx=AST_SX_SMALL;  break;
			default: break;
		}
		
		if (the_asteroids[i].size != AST_NONE)
		{
			x_cur = object_get_x(the_asteroids[i].asteroid_obj);
			y_cur = object_get_y(the_asteroids[i].asteroid_obj);
			x_prev = object_get_x_prev(
					the_asteroids[i].asteroid_obj);
			y_prev = object_get_y_prev(
					the_asteroids[i].asteroid_obj);
			state = object_get_state(the_asteroids[i].asteroid_obj);

			dx = OBJ2GFX( x_cur );
			dy = OBJ2GFX( y_cur );
			dx_prev = OBJ2GFX( x_prev );
			dy_prev = OBJ2GFX( y_prev );

			if ( (x_cur != x_prev) || (y_cur != y_prev) )
			{
				boxfill(dx_prev, dy_prev, 
						dx_prev+AST_TILE_SIZE-1, 
						dy_prev+AST_TILE_SIZE-1, 
						0, PSET);
				if(state != DYING)
					cpyv2v(sx+(animstep*AST_TILE_SIZE), 
						AST_SY, 
						sx+(animstep+1)*AST_TILE_SIZE-1,
						AST_SY+AST_TILE_SIZE-1,
						GFXPAGE, dx, dy, 0, TPSET);
			}
		}
	}
}

static void render_bullets()
{
	char i;
//	int sx = 0;
	int dx, dy;
	int dx_prev, dy_prev;
	int x_cur, y_cur;
	int x_prev, y_prev;
	
	for (i=0; i<MAX_NOF_BULLETS; i++)
	{
		if (the_bullets[i].bullet_obj != OBJ_VOID)
		{
			x_cur = object_get_x(the_bullets[i].bullet_obj);
			y_cur = object_get_y(the_bullets[i].bullet_obj);
			x_prev = object_get_x_prev(
					the_bullets[i].bullet_obj);
			y_prev = object_get_y_prev(
					the_bullets[i].bullet_obj);
			dx = OBJ2GFX( x_cur );
			dy = OBJ2GFX( y_cur );
			dx_prev = OBJ2GFX( x_prev );
			dy_prev = OBJ2GFX( y_prev );

			boxfill(dx_prev, dy_prev, 
					dx_prev+BULLET_TILE_SIZE-1, 
					dy_prev+BULLET_TILE_SIZE-1, 
					0, PSET);
			if (object_get_state(the_bullets[i].bullet_obj)
					!= DYING)
				boxfill(dx, dy, dx+BULLET_TILE_SIZE-1, 
					dy+BULLET_TILE_SIZE-1, 
					4, PSET); // temporary

/*			cpyv2v(sx+(BULLET_TILE_SIZE), BULLET_SY, 
			       sx+BULLET_TILE_SIZE-1, 
					BULLET_SY+BULLET_TILE_SIZE-1,
					GFXPAGE, dx, dy, 0, TPSET);*/
		}
	}
}

