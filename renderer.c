/* $Id: renderer.c,v 1.26 2003/05/29 21:56:46 manuel Exp $
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
#include "ship.h"
#include "ufo.h"
#include "asteroid.h"
#include "bullet.h"
#include "explosio.h"
#include "renderer.h"
#include "object.h"
#include "scores.h"
#include "font.h"

/*
 * LOCAL DEFINITIONS
 */

/* Gfx Coordinates */

#define BOOST_OFFSET 32

#define AST_SX_BIG      0
#define AST_SY_BIG     64
#define AST_SX_MEDIUM 128
#define AST_SY_MEDIUM  64
#define AST_SX_SMALL    0
#define AST_SY_SMALL   80
#define AST_SX_STEEL  128
#define AST_SY_STEEL   80

#define EXP_SX_BIG      0
#define EXP_SY_BIG     96
#define EXP_SX_MEDIUM 128
#define EXP_SY_MEDIUM  96
#define EXP_SX_SMALL    0
#define EXP_SY_SMALL  112

#define SHIELD_SX     128
#define SHIELD_SY     112

#define UFO_SX          0
#define UFO_SY        128

#define GAMEPAGE 0
#define BGPAGE  3

// GFXPAGE: see renderer.h

#define SCORE_Y (212-FONT_H+1) // go to the edge; compensate for empty line
#define SCORE_X 0
#define LIVES_Y (SCORE_Y)
#define LIVES_X (256-8*(FONT_W))

#define SHIELD_O_METER_H 5
#define SHIELD_O_METER_Y (212-SHIELD_O_METER_H-1)
#define SHIELD_O_METER_X ((256-64)>>1)

#define	C_TRANSP 0
#define C_BLACK 1
#define	C_SHIELD_O_METER 14
#define	C_WHITE 15

static int palette[] = 
{	/*-GRB*/
	0x0000,
	0x0000, /* 1: black                    */
	0x0330, /* 2:                          */
	0x0330, /* 3:                          */
	0x0337, /* 4:                          */
	0x0330, /* 5:                          */
	0x0330, /* 6:                          */
	0x0330, /* 7:                          */
	0x0771, /* 8: yellow    ufo            */
	0x0107, /* 9: d blue    shield         */
	0x0327, /*10: l blue    shield         */
	0x0664, /*11: l yellow  explosion/ufo  */
	0x0222, /*12: d grey    all            */
	0x0333, /*13: m grey    all            */
	0x0555, /*14: l grey    all            */
	0x0777  /*15: white     all            */
};

int gs2loadgrp(uchar scrnmode, uchar page, char *filename);

static void render_ship(onoff_t boost, onoff_t shield);
static void render_ufo();
static void render_asteroids();
static void render_explosions();
static void render_bullets();
static void generate_background();

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
	
	for(i=0; i < 25; i++)
	{
		pset(i*6, 0, 15, TPRESET);
		grpprt("Loading GFX - Plz wait..."[i],PSET);
	}
	strcpy(filename,"SHIPS.SR5"); 
	gs2loadgrp(5, GFXPAGE, filename);
	boxfill(0,0, 255,211, 0, PSET); // wipe screen
	*Timer=0;
}

void playscreen_init()
{
	uchar i;
	for(i=0; i<16;i++)
	{
		setplt(i, palette[i]);
	}

	generate_background();
	
	setpg(GAMEPAGE,GAMEPAGE);
	
	write("Score:", SCORE_X, SCORE_Y);
	write("Ships:", LIVES_X, LIVES_Y);

	boxline (SHIELD_O_METER_X,SHIELD_O_METER_Y, 
		 SHIELD_O_METER_X+63+1,SHIELD_O_METER_Y+SHIELD_O_METER_H-1, 
		 C_WHITE, PSET);
	
}
	

static void generate_background()
{
	int i;
	setpg(GAMEPAGE,BGPAGE);
	cls();
	for (i=0; i<100; i++)
	{
		pset(rand()%256,rand()%(212-8),rand()%14+2,PSET);
	}
	boxline(0,0, 255,211-8, 1, PSET);
	cpyv2v(0,0, 255,211, BGPAGE, 0,0, GAMEPAGE, PSET);
}
	

void render_frame(onoff_t boost, onoff_t shield, char noflives)
{
#ifdef DEBUG_FPS
	line(0,0,25,0,1,PSET);
	line(0,0,*Timer,0,15,PSET);
	pset(4,1,8,PSET);
	pset(9,1,8,PSET);
#endif
#ifdef DEBUG_RENDERER
	{
		int i;
		for(i=0; i < 50; i++)
		{
			pset(i,0,object_get_type(i)+2, PSET);
		}
	}
#endif
	while (*Timer<6);
	*Timer=0;
	frame_counter++;
	if (the_ship.ship_obj!=OBJ_VOID) render_ship(boost, shield);
	render_ufo();
	render_asteroids();
	render_bullets();
	render_explosions();
	render_info(noflives);
}

static void render_ship(onoff_t boost, onoff_t shield)
{
	int sx, sy;
	int dx, dy;
	int dx_prev, dy_prev;
	int anim_step = (frame_counter % 8);
	int tilesize;
	obj_hdl_t ship_obj = the_ship.ship_obj;
	
	int x_cur = object_get_x(ship_obj);
	int y_cur = object_get_y(ship_obj);
	int x_prev = object_get_x_prev(ship_obj);
	int y_prev = object_get_y_prev(ship_obj);
	static onoff_t shield_prev=0;

	if ((x_cur != x_prev) || (y_cur != y_prev) ||
	     (the_ship.heading != the_ship.heading_prev) ||
	     (object_get_state(ship_obj) == NEW)||
	     (object_get_state(ship_obj) == DYING)|| shield || shield_prev)
	{
		tilesize = OBJ2GFX(object_get_size(ship_obj)) - 1;
		if (object_get_state(ship_obj) == NEW)
			object_set_state(ship_obj, ALIVE);
		else
		{
			dx_prev = OBJ2GFX(x_prev);
			dy_prev = OBJ2GFX(y_prev);
			cpyv2v(dx_prev, dy_prev,
			       dx_prev+tilesize, dy_prev+tilesize, BGPAGE,
			       dx_prev, dy_prev, GAMEPAGE, PSET);
		}
		if (object_get_state(ship_obj) != DYING)
		{
			dx = OBJ2GFX(x_cur);
			dy = OBJ2GFX(y_cur);
			sx = (the_ship.heading & 0x0F) << 4; 
			sy = the_ship.heading & 0xF0; 
			if (boost == ON) sy+=BOOST_OFFSET;
			cpyv2v(sx, sy, sx+tilesize, sy+tilesize, GFXPAGE, dx, dy, GAMEPAGE,
							TPSET);
			if(shield)
			{
				sx = SHIELD_SX + SHIELD_TILE_SIZE * anim_step;
				cpyv2v(sx, SHIELD_SY, sx+SHIELD_TILE_SIZE-1,
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
	int sx = 0, sy;
	int dx, dy;
	int dx_prev, dy_prev;
	int x_cur, y_cur;
	int x_prev, y_prev;
	int tilesize, offset;
	char animstep = frame_counter%4;
	state_e state;
	char counter=nof_asteroids;
	
	for (i=0; i<MAX_NOF_ASTEROIDS && counter; i++)
	{
		
		if (the_asteroids[i].size != AST_NONE)
		{
			counter--;
			x_cur = object_get_x(the_asteroids[i].asteroid_obj);
			y_cur = object_get_y(the_asteroids[i].asteroid_obj);
			x_prev = object_get_x_prev(the_asteroids[i].asteroid_obj);
			y_prev = object_get_y_prev(the_asteroids[i].asteroid_obj);

			tilesize = OBJ2GFX(object_get_size(the_asteroids[i].asteroid_obj));

			dx_prev = OBJ2GFX(x_prev);
			dy_prev = OBJ2GFX(y_prev);

			tilesize--;
			cpyv2v(dx_prev, dy_prev, dx_prev+tilesize, dy_prev+tilesize, 
			       BGPAGE, dx_prev, dy_prev, GAMEPAGE, PSET);
				
			state = object_get_state(the_asteroids[i].asteroid_obj);
			if(state != DYING)
			{
				switch (the_asteroids[i].size)
				{
					case AST_BIG:
						if (the_asteroids[i].type!=AST_TYPE_NORMAL)
						{
							sx=AST_SX_STEEL;
							sy=AST_SY_STEEL;
						} else {
							sx=AST_SX_BIG;
							sy=AST_SY_BIG;
						}
						break;
				 	case AST_MEDIUM:
						sx=AST_SX_MEDIUM;
						sy=AST_SY_MEDIUM;
						break;
					case AST_SMALL:
						sx=AST_SX_SMALL;
						sy=AST_SY_SMALL;
						break;
					default:
						break;
				}
				animstep=(animstep+i)%8; // variation
				offset = (AST_TILE_SIZE - tilesize)>>1;
				sx += animstep*AST_TILE_SIZE + offset;
				sy += offset;
				dx = OBJ2GFX(x_cur);
				dy = OBJ2GFX(y_cur);
				cpyv2v(sx, sy, sx+tilesize, sy+tilesize,
				       GFXPAGE, dx, dy, GAMEPAGE, TPSET);
			}
		}
	}
}

static void render_bullet_obj(obj_hdl_t bullet_obj)
{
	int dx, dy;
	int dx_prev, dy_prev;
	int x_cur, y_cur;
	int x_prev, y_prev;
	int size;
	
	if (bullet_obj != OBJ_VOID)
	{
		x_cur = object_get_x(bullet_obj);
		y_cur = object_get_y(bullet_obj);
		x_prev = object_get_x_prev(bullet_obj);
		y_prev = object_get_y_prev(bullet_obj);
		dx_prev = OBJ2GFX(x_prev);
		dy_prev = OBJ2GFX(y_prev);
		size = OBJ2GFX(object_get_size(bullet_obj)) - 1;

		cpyv2v(dx_prev, dy_prev, dx_prev+size, dy_prev+size, 
			       BGPAGE, dx_prev, dy_prev, GAMEPAGE, PSET);
		if (object_get_state(bullet_obj) != DYING)
		{
			dx = OBJ2GFX(x_cur);
			dy = OBJ2GFX(y_cur);
			boxfill(dx, dy, dx+size, dy+size, 15, PSET); // temporary

/*			cpyv2v(BULLET_SX, BULLET_SY, BULLET_SX+size, BULLET_SY+size,
				       GFXPAGE, dx, dy, GAMEPAGE, TPSET);*/
		}
	}
}

static void render_bullets()
{
	char i;
	for (i=0; i<MAX_NOF_BULLETS; i++)
	{
		render_bullet_obj(the_bullets[i].bullet_obj);
	}
	render_bullet_obj(the_ufo_bullet.bullet_obj);
}

static void render_explosions()
{
	char i;
	int sx = 0, sy;
	int dx, dy;
	int dx_prev, dy_prev;
	int x_cur, y_cur;
	int x_prev, y_prev;
	int tilesize, offset;
	char animstep;
	state_e state;
	char counter=nof_explosions;
	
	for (i=0; i<MAX_NOF_EXPLOSIONS && counter; i++)
	{
		if (the_explosions[i].explosion_obj != OBJ_VOID)
		{
			counter--;
			x_cur = object_get_x(the_explosions[i].explosion_obj);
			y_cur = object_get_y(the_explosions[i].explosion_obj);
			x_prev = object_get_x_prev(the_explosions[i].explosion_obj);
			y_prev = object_get_y_prev(the_explosions[i].explosion_obj);

			if ((x_cur != x_prev) || (y_cur != y_prev))
			{
				tilesize = OBJ2GFX(object_get_size(
					the_explosions[i].explosion_obj));
				
				dx_prev = OBJ2GFX(x_prev);
				dy_prev = OBJ2GFX(y_prev);

				tilesize--;
				cpyv2v(dx_prev, dy_prev, dx_prev+tilesize, dy_prev+tilesize, 
				       BGPAGE, dx_prev, dy_prev, GAMEPAGE, PSET);

				state = object_get_state(the_explosions[i].explosion_obj);
				if(state != DYING)
				{
					switch (the_explosions[i].size)
					{
					case EXP_BIG:
						sx = EXP_SX_BIG;
						sy = EXP_SY_BIG;
						break;
				 	case EXP_MEDIUM:
						sx = EXP_SX_MEDIUM;
						sy = EXP_SY_MEDIUM;
						break;
					case EXP_SMALL:
						sx = EXP_SX_SMALL;
						sy = EXP_SY_SMALL;
						break;
					default:
						break;
					}
					animstep= NEW_EXPLOSION_AGE - the_explosions[i].age;
					offset = (EXP_TILE_SIZE - tilesize)>>1;
					sx += animstep*EXP_TILE_SIZE + offset;
					sy += offset;
					dx = OBJ2GFX(x_cur);
					dy = OBJ2GFX(y_cur);
					cpyv2v(sx, sy, sx+tilesize, sy+tilesize, GFXPAGE, dx, dy, 
									GAMEPAGE, TPSET);
				}
			}
		}
	}
}


static void render_ufo ()
{
	int sx, sy;
	int dx, dy;
	int x_cur, y_cur;
	int x_prev, y_prev;
	int tilesize;
	state_e state;
	
	int anim_step = frame_counter % 4;
	obj_hdl_t ufo_obj = the_ufo.ufo_obj;

	if (ufo_obj != OBJ_VOID)
	{
		x_cur = object_get_x(ufo_obj);
		y_cur = object_get_y(ufo_obj);
		x_prev = object_get_x_prev(ufo_obj);
		y_prev = object_get_y_prev(ufo_obj);
		tilesize = OBJ2GFX(object_get_size(ufo_obj)) - 1;
		dx = OBJ2GFX(x_prev);
		dy = OBJ2GFX(y_prev);
		cpyv2v(dx,dy, dx+tilesize, dy+tilesize, BGPAGE, dx,dy, GAMEPAGE, PSET);

		state = object_get_state(ufo_obj);
		if(state != DYING)
		{
			dx = OBJ2GFX(x_cur);
			dy = OBJ2GFX(y_cur);
			sx = UFO_SX + anim_step * (tilesize + 1);
			sy = UFO_SY;
			cpyv2v(sx, sy, sx+tilesize, sy+tilesize, GFXPAGE, dx, dy, GAMEPAGE,
							TPSET);
		}
	}
}

			
void render_info(char noflives)
{
	char shield_stat = the_ship.shield_energy >> 2;
	char str[7+6];
	
	if (shield_stat>0)
		boxfill (SHIELD_O_METER_X+1,SHIELD_O_METER_Y+1,
			SHIELD_O_METER_X+shield_stat,
			SHIELD_O_METER_Y+SHIELD_O_METER_H-2, 
			C_SHIELD_O_METER, PSET);
	if (shield_stat<63)
		boxfill (SHIELD_O_METER_X+1+shield_stat,SHIELD_O_METER_Y+1, 
			SHIELD_O_METER_X+63,
			SHIELD_O_METER_Y+SHIELD_O_METER_H-2, C_BLACK, PSET);
	
	sprintf(str, "%05d", score);
	write(str, SCORE_X+7*FONT_W, SCORE_Y);
	
	sprintf(str, "%d", noflives);
	write(str, LIVES_X+7*FONT_W, LIVES_Y);
}


