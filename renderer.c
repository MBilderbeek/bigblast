/* $Id: renderer.c,v 1.31 2004/12/03 23:04:48 manuel Exp $
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

static int palette_game[128];
static int palette_menu[128];

static void render_ship(onoff_t boost, onoff_t shield);
static void unrender_ship();
static void render_ufo();
static void unrender_ufo();
static void render_asteroids();
static void unrender_asteroids();
static void render_explosions();
static void unrender_explosions();
static void render_bullets();
static void unrender_bullets();
static void generate_background();

#define JIFFY (*((int *)0xFC9E))

unsigned int frame_counter;

extern int gs2loadgrp(char *,  uchar);
extern int gs2loadpal(char *, int *);
/*
 * EXTERNAL FUNCTIONS
 */

void render_init()
{
	uchar i;

	ginit();
	color(15,0,0);
	screen(5);
	
	for(i=0; i < 25; i++)
	{
		pset(i*6, 0, 15, TPRESET);
		grpprt("Loading GFX - Plz wait..."[i],PSET);
	}
	
	
	(void) gs2loadgrp("ships.sr5", GFXPAGE);
	(void) gs2loadpal("ships.pl5", palette_game);
	
	JIFFY = 0;
}

void menuscreen_init()
{
	setpg(0,0);
	db_stop();
	boxfill(0,0, 255,211, 0, PSET); // wipe screen
	setpg(0,1);
	boxfill(0,0, 255,211, 0, PSET); // wipe screen
	setpg(0,0);
	(void) gs2loadgrp("intro.sr5", 0);
	(void) gs2loadpal("intro.pl5", palette_menu);
			// we could also just load it once,
			// but it takes so little time that it isn't worth it
}

void playscreen_init()
{
	uchar i;
	for(i=0; i<16;i++)
	{
		setplt(i, palette_game[i]);
	}
	
	db_start();
	boxfill(0,0, 255,211, 0, PSET); // wipe screen
	db_swap();
	boxfill(0,0, 255,211, 0, PSET); // wipe screen

	generate_background();
	
	write("Score:", SCORE_X, SCORE_Y);
	write("Ships:", LIVES_X, LIVES_Y);

	boxline (SHIELD_O_METER_X,SHIELD_O_METER_Y, 
		 SHIELD_O_METER_X+63+1,SHIELD_O_METER_Y+SHIELD_O_METER_H-1, 
		 C_WHITE, PSET);

	cpyv2v(0,0,255,211,c_apage,0,0,c_dpage,PSET);
	
	setpg(GAMEPAGE,1-GAMEPAGE);
}
	

static void generate_background()
{
	int i, p=c_apage;
	setpg(c_dpage,BGPAGE);
	cls();
	for (i=0; i<100; i++)
	{
		pset(rand()%256,rand()%(212-8),rand()%14+2,PSET);
	}
	boxline(0,0, 255,211-8, 1, PSET);
	cpyv2v(0,0, 255,211, BGPAGE, 0,0, p, PSET);
	setpg(c_dpage,p);
}
	

void render_frame(onoff_t boost, onoff_t shield, char noflives)
{
#ifdef DEBUG_FPS
	line(0,0,25,0,1,PSET);
	line(0,0,JIFFY,0,15,PSET);
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
	while (JIFFY<6);
	JIFFY=0;
	db_swap();

	if (the_ship.ship_obj!=OBJ_VOID) unrender_ship();
	frame_counter++;
	unrender_ufo();
	unrender_asteroids();
	unrender_bullets();
	unrender_explosions();
	if (the_ship.ship_obj!=OBJ_VOID) render_ship(boost, shield);
	render_ufo();
	render_asteroids();
	render_bullets();
	render_explosions();
	render_info(noflives);
}

static void unrender_ship()
{
	int dx,dy;
	obj_hdl_t ship_obj = the_ship.ship_obj;
	int tilesize = OBJ2GFX(object_get_size(ship_obj)) - 1;

	if (object_get_state(ship_obj) != NEW)
	{
		dx = OBJ2GFX( object_get_x_prev(ship_obj) );
		dy = OBJ2GFX( object_get_y_prev(ship_obj) );
		cpyv2v(dx, dy, dx+tilesize, dy+tilesize, BGPAGE,
		       dx, dy, c_apage, PSET);
	}
	if (object_get_state(ship_obj) == DYING)
	{
		dx = OBJ2GFX ( object_get_x_prev_op(ship_obj) );
		dy = OBJ2GFX ( object_get_y_prev_op(ship_obj) );
		
		cpyv2v(dx, dy, dx+tilesize, dy+tilesize, BGPAGE,
		       dx, dy, c_dpage, PSET);		
	}
}

static void render_ship(onoff_t boost, onoff_t shield)
{
	obj_hdl_t ship_obj = the_ship.ship_obj;
	int tilesize = OBJ2GFX(object_get_size(ship_obj)) - 1;

	if (object_get_state(ship_obj) == NEW)
		object_set_state(ship_obj, ALIVE);
	if (object_get_state(ship_obj) != DYING)
	{
		int dx = OBJ2GFX( object_get_x(ship_obj) );
		int dy = OBJ2GFX( object_get_y(ship_obj) );
		int sx = (the_ship.heading & 0x0F) << 4; 
		int sy = the_ship.heading & 0xF0; 
		if (boost == ON) sy+=BOOST_OFFSET;
		cpyv2v(sx, sy, sx+tilesize, sy+tilesize, GFXPAGE, dx, dy, c_apage,
						TPSET);
		if(shield)
		{
			int anim_step = (frame_counter % 8);
			sx = SHIELD_SX + SHIELD_TILE_SIZE * anim_step;
			cpyv2v(sx, SHIELD_SY, sx+SHIELD_TILE_SIZE-1,
			         SHIELD_SY + SHIELD_TILE_SIZE - 1,
			       GFXPAGE, dx, dy, c_apage, TPSET);
		}
	}
}
	
static void unrender_asteroids()
{
	char i;
	char counter=nof_asteroids;
	
	for (i=0; i<MAX_NOF_ASTEROIDS && counter; i++)
	{
		if (the_asteroids[i].size != AST_NONE)
		{
			int dx = OBJ2GFX( object_get_x_prev(the_asteroids[i].asteroid_obj) );
			int dy = OBJ2GFX( object_get_y_prev(the_asteroids[i].asteroid_obj) );

			int tilesize = OBJ2GFX(object_get_size(the_asteroids[i].asteroid_obj)) -1;
			counter--;

			cpyv2v(dx, dy, dx+tilesize, dy+tilesize, 
			       BGPAGE, dx, dy, c_apage, PSET);
				
			if(object_get_state(the_asteroids[i].asteroid_obj) == DYING)
			{
				dx = OBJ2GFX( object_get_x_prev_op(the_asteroids[i].asteroid_obj) );
				dy = OBJ2GFX( object_get_y_prev_op(the_asteroids[i].asteroid_obj) );
				cpyv2v(dx, dy, dx+tilesize, dy+tilesize, 
			           BGPAGE, dx, dy, c_dpage, PSET);
			}
		}
	}
}
static void render_asteroids()
{
	char i;
	char counter=nof_asteroids;
	char animstep = frame_counter%4;

	for (i=0; i<MAX_NOF_ASTEROIDS && counter; i++)
	{

		if (the_asteroids[i].size != AST_NONE)
		{
			counter--;
			if(object_get_state(the_asteroids[i].asteroid_obj) != DYING)
			{
				int dx = OBJ2GFX( object_get_x(the_asteroids[i].asteroid_obj) );
				int dy = OBJ2GFX( object_get_y(the_asteroids[i].asteroid_obj) );
				int tilesize = OBJ2GFX(object_get_size(the_asteroids[i].asteroid_obj)) -1;
				int offset = (AST_TILE_SIZE - tilesize)>>1;
				int sx = 0, sy;
				animstep=(animstep+i)%8; // variation
				
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
				sx += animstep*AST_TILE_SIZE + offset;
				sy += offset;
				cpyv2v(sx, sy, sx+tilesize, sy+tilesize,
				       GFXPAGE, dx, dy, c_apage, TPSET);
			}
		}
	}
}

static void unrender_bullet_obj(obj_hdl_t bullet_obj)
{
	if (bullet_obj != OBJ_VOID)
	{
		int dx = OBJ2GFX( object_get_x_prev(bullet_obj) );
		int dy = OBJ2GFX( object_get_y_prev(bullet_obj) );
		int size = OBJ2GFX(object_get_size(bullet_obj)) - 1;

		cpyv2v(dx, dy, dx+size, dy+size, 
			       BGPAGE, dx, dy, c_apage, PSET);
		if (object_get_state(bullet_obj) == DYING)
		{
			dx = OBJ2GFX( object_get_x_prev_op(bullet_obj) );
			dy = OBJ2GFX( object_get_y_prev_op(bullet_obj) );
			cpyv2v(dx, dy, dx+size, dy+size, 
			       BGPAGE, dx, dy, c_dpage, PSET);
		}
	}
}

static void render_bullet_obj(obj_hdl_t bullet_obj)
{
	if (bullet_obj != OBJ_VOID && object_get_state(bullet_obj) != DYING)
	{
		int dx = OBJ2GFX( object_get_x(bullet_obj) );
		int dy = OBJ2GFX( object_get_y(bullet_obj) );
		int size = OBJ2GFX(object_get_size(bullet_obj)) - 1;

		boxfill(dx, dy, dx+size, dy+size, 15, PSET);
	}
}

static void unrender_bullets()
{
	char i;
	for (i=0; i<MAX_NOF_BULLETS; i++)
	{
		unrender_bullet_obj(the_bullets[i].bullet_obj);
	}
	unrender_bullet_obj(the_ufo_bullet.bullet_obj);
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


static void unrender_explosions()
{
	char i;
	char counter=nof_explosions;
	
	for (i=0; i<MAX_NOF_EXPLOSIONS && counter; i++)
	{
		if (the_explosions[i].explosion_obj != OBJ_VOID)
		{
			int dx = OBJ2GFX( object_get_x_prev(the_explosions[i].explosion_obj) );
			int dy = OBJ2GFX( object_get_y_prev(the_explosions[i].explosion_obj) );

			int tilesize = OBJ2GFX(object_get_size(
					the_explosions[i].explosion_obj)) -1;
				
			counter--;
			cpyv2v(dx, dy, dx+tilesize, dy+tilesize, 
			       BGPAGE, dx, dy, c_apage, PSET);

			if(object_get_state(the_explosions[i].explosion_obj) == DYING)
			{
				dx = OBJ2GFX( object_get_x_prev_op(the_explosions[i].explosion_obj) );
				dy = OBJ2GFX( object_get_y_prev_op(the_explosions[i].explosion_obj) );
				cpyv2v(dx, dy, dx+tilesize, dy+tilesize,
							BGPAGE, dx, dy, c_dpage, PSET);
			}
		}
	}
}

static void render_explosions()
{
	char i;
	char counter=nof_explosions;
	
	for (i=0; i<MAX_NOF_EXPLOSIONS && counter; i++)
	{
		if (the_explosions[i].explosion_obj != OBJ_VOID)
		{
			int sx = 0, sy;
			counter--;
			if(object_get_state(the_explosions[i].explosion_obj) != DYING)
			{
				int dx = OBJ2GFX(object_get_x(the_explosions[i].explosion_obj));
				int dy = OBJ2GFX(object_get_y(the_explosions[i].explosion_obj));
				int tilesize = OBJ2GFX(object_get_size(
					the_explosions[i].explosion_obj)) -1;
				char animstep= NEW_EXPLOSION_AGE - the_explosions[i].age;
				int offset = (EXP_TILE_SIZE - tilesize)>>1;
			
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
				sx += animstep*EXP_TILE_SIZE + offset;
				sy += offset;
				cpyv2v(sx, sy, sx+tilesize, sy+tilesize, GFXPAGE, dx, dy, 
								c_apage, TPSET);
			}
		}
	}
}


static void unrender_ufo ()
{
	obj_hdl_t ufo_obj = the_ufo.ufo_obj;

	if (ufo_obj != OBJ_VOID)
	{
		int tilesize = OBJ2GFX(object_get_size(ufo_obj)) - 1;
		int dx = OBJ2GFX( object_get_x_prev(ufo_obj) );
		int dy = OBJ2GFX( object_get_y_prev(ufo_obj) );
		cpyv2v(dx,dy, dx+tilesize, dy+tilesize, BGPAGE, dx,dy, c_apage, PSET);
		if(object_get_state(ufo_obj) == DYING)
		{
			dx = OBJ2GFX( object_get_x_prev_op(ufo_obj) );
			dy = OBJ2GFX( object_get_y_prev_op(ufo_obj) );
			cpyv2v(dx,dy, dx+tilesize, dy+tilesize, BGPAGE, dx,dy, c_dpage, PSET);
		}
	}
}

static void render_ufo ()
{
	obj_hdl_t ufo_obj = the_ufo.ufo_obj;

	if (ufo_obj != OBJ_VOID && (object_get_state(ufo_obj) != DYING) )
	{
		int tilesize = OBJ2GFX(object_get_size(ufo_obj)) - 1;
		int dx = OBJ2GFX( object_get_x(ufo_obj) );
		int dy = OBJ2GFX( object_get_y(ufo_obj) );
		int anim_step = frame_counter % 4;
		int sx = UFO_SX + anim_step * (tilesize + 1);
		int sy = UFO_SY;
		cpyv2v(sx, sy, sx+tilesize, sy+tilesize, GFXPAGE, dx, dy, 
				c_apage, TPSET);
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


