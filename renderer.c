/* $Id: renderer.c,v 1.11 2002/11/21 23:46:58 manuel Exp $
 *
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to render frames. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include "stdlib.h"
#include "glib.h"
#include "string.h"
#include "ship.h"
#include "asteroid.h"
#include "bullet.h"
#include "renderer.h"
#include "object.h"
#include "scores.h"

/*
 * LOCAL DEFINITIONS
 */

#define BOOST_OFFSET 32

#define AST_SX_BIG 0
#define AST_SX_MEDIUM (4*AST_TILE_SIZE)
#define AST_SX_SMALL (8*AST_TILE_SIZE)
#define AST_SY (4*SHIP_TILE_SIZE)
#define SHIELD_SY (AST_SY + 16)
#define GFXPAGE 2
#define BGPAGE  3
#define GAMEPAGE 0

#define FONT_Y 188
#define FONT_W 6
#define FONT_H 8
#define FONT_CPL 42 // Nof characters on a line

#define SCORE_Y (212-FONT_H+1) // go to the edge; compensate for empty line
#define SCORE_X 0
#define LIVES_Y (SCORE_Y)
#define LIVES_X (256-8*(FONT_W))

#define SHIELD_O_METER_H 5
#define SHIELD_O_METER_Y (212-SHIELD_O_METER_H-1)
#define SHIELD_O_METER_X ((256-64)>>1)

#define	C_TRANSP 0
#define C_BLACK 1
#define	C_SHIELD1 2
#define	C_SHIELD2 3
#define	C_SHIELD3 4
#define	C_SHIELD_O_METER 14
#define	C_WHITE 15

static int palette[] = 
{	/*-GRB*/
	0x0000,
	0x0111, /* 1: black    */
	0x0707, /* 2: shield 1 */
	0x0404, /* 3: shield 2 */
	0x0202, /* 4: shield 3 */
	0x0333, /* 5:          */
	0x0070, /* 6:          */
	0x0527, /* 7:          */
	0x0527, /* 8:          */
	0x0572, /* 9:          */
	0x0333, /*10:          */
	0x0333, /*11:          */
	0x0333, /*12:          */
	0x0333, /*13:          */
	0x0117, /*14:          */
	0x0777  /*15:          */
};

extern void loadgrp(char *filename, unsigned int x, unsigned char y, char page);

static void render_ship(onoff_t boost, onoff_t shield);
static void render_asteroids();
static void render_bullets();
static void generate_background();
static void render_info(char noflives);

int *Timer=(int *)0xFC9E;                                /*Systeemtimer*/

unsigned int frame_counter;

/*
 * EXTERNAL FUNCTIONS
 */

void render_init()
{
	uchar i;
	char filename[16];
	
	ginit();
	color(15,0,0);
	screen(5);
	for(i=0; i<16;i++)
	{
		setplt(i, palette[i]);
	}
	
	for(i=0; i < 25; i++)
	{
		pset(i*6, 0, 15, TPRESET);
		grpprt("Loading GFX - Plz wait..."[i],PSET);
	}
	strcpy(filename,"\"SHIPS.COP\""); 
	loadgrp(filename, 0, 0, GFXPAGE);
	boxfill(0,0, 255,211, 0, PSET); // wipe screen
}

void playscreen_init()
{
	setpg(GAMEPAGE,BGPAGE);
	generate_background();
	boxline(0,0, 255,211-8, 1, PSET);
	
	cpyv2v(0,0, 255,211, BGPAGE, 0,0, GAMEPAGE, PSET);
	
	write("Score:", SCORE_X, SCORE_Y);
	write("Lives:", LIVES_X, LIVES_Y);

	setpg(GAMEPAGE,GAMEPAGE);
	
	boxline (SHIELD_O_METER_X,SHIELD_O_METER_Y, 
		 SHIELD_O_METER_X+63+2,SHIELD_O_METER_Y+SHIELD_O_METER_H-1, 
		 C_WHITE, PSET);
	
}
	

static void generate_background()
{
	int i;
	cls();
	for (i=0; i<100; i++)
	{
		pset(rand()%256,rand()%(212-8),rand()%14+2,PSET);
	}
}
	

void render_frame(onoff_t boost, onoff_t shield, char noflives)
{
	while (*Timer<5);
	*Timer=0;
	frame_counter++;
	render_ship(boost, shield);
	render_asteroids();
	render_bullets();
	render_info(noflives);
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

static void render_ship(onoff_t boost, onoff_t shield)
{
	int sx, sy;
	int dx, dy;
	int dx_prev, dy_prev;
	int anim_step = (frame_counter % 4);
	
	int x_cur = object_get_x(the_ship.ship_obj);
	int y_cur = object_get_y(the_ship.ship_obj);
	int x_prev = object_get_x_prev(the_ship.ship_obj);
	int y_prev = object_get_y_prev(the_ship.ship_obj);
	static onoff_t shield_prev;

	if (the_ship.shield_energy > 0 && shield == ON)
		the_ship.shield_energy--;
	
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
	     (object_get_state(the_ship.ship_obj) == NEW)||
	     (object_get_state(the_ship.ship_obj) == DYING)||
	     shield || shield_prev )
	{
		if ( object_get_state(the_ship.ship_obj) == NEW )
			object_set_state(the_ship.ship_obj, ALIVE);
		else
			cpyv2v(dx_prev, dy_prev, dx_prev+SHIP_TILE_SIZE-1, 
				dy_prev+SHIP_TILE_SIZE-1, BGPAGE,
			       dx_prev, dy_prev, GAMEPAGE, PSET);
		if (object_get_state(the_ship.ship_obj) != DYING)
		{
			cpyv2v(sx, sy, sx+SHIP_TILE_SIZE-1, sy+SHIP_TILE_SIZE-1,
				GFXPAGE, dx, dy, GAMEPAGE, TPSET);
			if(shield)
			{
				sx = SHIELD_TILE_SIZE * anim_step;
				cpyv2v(sx, SHIELD_SY,
				       sx+SHIELD_TILE_SIZE-1,
				         SHIELD_SY + SHIELD_TILE_SIZE - 1,
				       GFXPAGE, dx, dy, GAMEPAGE, TPSET);
			}
		}
		shield_prev = shield;
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
				cpyv2v(dx_prev, dy_prev, 
				       dx_prev+AST_TILE_SIZE-1, 
				       dy_prev+AST_TILE_SIZE-1, 
				       BGPAGE, dx_prev, dy_prev, GAMEPAGE,
				       PSET);
				if(state != DYING)
					cpyv2v(sx+(animstep*AST_TILE_SIZE), 
						AST_SY, 
						sx+(animstep+1)*AST_TILE_SIZE-1,
						AST_SY+AST_TILE_SIZE-1,
						GFXPAGE, dx, dy, GAMEPAGE,
						TPSET);
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

			cpyv2v(dx_prev, dy_prev, 
			       dx_prev+BULLET_TILE_SIZE-1, 
			       dy_prev+BULLET_TILE_SIZE-1, 
			       BGPAGE, dx_prev, dy_prev, GAMEPAGE, PSET);
			if (object_get_state(the_bullets[i].bullet_obj)
					!= DYING)
				boxfill(dx, dy, dx+BULLET_TILE_SIZE-1, 
					dy+BULLET_TILE_SIZE-1, 
					15, PSET); // temporary

/*			cpyv2v(sx+(BULLET_TILE_SIZE), BULLET_SY, 
			       sx+BULLET_TILE_SIZE-1, 
					BULLET_SY+BULLET_TILE_SIZE-1,
					GFXPAGE, dx, dy, GAMEPAGE, TPSET);*/
		}
	}
}

void write_cent(char *string, unsigned int y)
{
	write(string, (255-(strlen(string)*FONT_W)) >> 1, y);
}

void write(char *string, unsigned int x, unsigned int y)
{
	unsigned char i,charnumber,sx,sy;
	
	for (i=0; i<strlen(string); i++)
	{
		if (string[i]>=32 && string[i]<=127)
		{
			charnumber=(string[i]-32);
			sx=(charnumber%FONT_CPL)*FONT_W;
			sy=FONT_Y+(charnumber/FONT_CPL)*FONT_H;
			cpyv2v(sx, sy, sx+FONT_W-1, sy+FONT_H-1, GFXPAGE,
					x+i*FONT_W, y, GAMEPAGE, PSET);
		}
	}
}
					
static void render_info(char noflives)
{
	char shield_stat = the_ship.shield_energy >> 2;
	char str[7+6];
	
	if (shield_stat>0)
		boxfill (SHIELD_O_METER_X+1,SHIELD_O_METER_Y+1,
			SHIELD_O_METER_X+shield_stat,
			SHIELD_O_METER_Y+SHIELD_O_METER_H-2, 
			C_SHIELD_O_METER, PSET);
	if (shield_stat<63)
		boxfill (SHIELD_O_METER_X+shield_stat+1,SHIELD_O_METER_Y+1, 
			SHIELD_O_METER_X+64-2,
			SHIELD_O_METER_Y+SHIELD_O_METER_H-2, C_BLACK, PSET);
	
	sprintf(str, "%05d", score);
	write(str, SCORE_X+7*FONT_W, SCORE_Y);
	
	sprintf(str, "%d", noflives);
	write(str, LIVES_X+7*FONT_W, LIVES_Y);
}


