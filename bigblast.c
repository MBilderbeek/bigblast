/* $Id: bigblast.c,v 1.9 2002/12/26 17:04:00 manuel Exp $
 *
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains the main. Yup. That's it.
 */

/*
 * INCLUDES
 */
#include "stdlib.h"
#include "renderer.h"
#include "object.h"
#include "ship.h"
#include "asteroid.h"
#include "collisio.h"
#include "scores.h"
//#include "msxbios.h"

/*
 * LOCAL DEFINITIONS
 */
#define MSG_BASE 60
#define MSG_BONUS_BASE 80

int *JIFFY = (int *)0xFC9E;
unsigned int score;
unsigned char level;
static char quit=0;
static char noflives=9;

static int get_rnd_coord(int range)
{
	int netrange = range - GFX2OBJ(SHIP_TILE_SIZE);
	int coord = rand() %(netrange);
	if (coord > (netrange >> 1)) 
		coord += GFX2OBJ(SHIP_TILE_SIZE);
	return(coord);
}

static void add_bonus(unsigned char level, unsigned char noflives)
{
	char string[100];
	unsigned int levelbonus, timebonus, shieldbonus, lifebonus;
	
	sprintf(string,"BONUS");
	write_cent(string, MSG_BONUS_BASE);
	
	levelbonus=SC_LEVEL(level);
	score+=levelbonus;
	sprintf(string,"Level bonus: %d", levelbonus);
	write_cent(string, MSG_BONUS_BASE + 10);
	
	if (frame_counter<TIME_BONUS)
		timebonus=(TIME_BONUS-frame_counter);
	else
		timebonus=0;
	score+=timebonus;
	sprintf(string,"Time bonus: %d", timebonus);
	write_cent(string, MSG_BONUS_BASE + 20);

//	if (bullet_counter<BULLET_BONUS(level)) score+=(BULLET_BONUS(level)-bullet_counter);

	shieldbonus=the_ship.shield_energy;
	score+=shieldbonus;
	sprintf(string,"Shield bonus: %d", shieldbonus);
	write_cent(string, MSG_BONUS_BASE + 30);
	
	lifebonus=noflives*10;
	score+=lifebonus;
	sprintf(string,"Life bonus: %d", lifebonus);
	write_cent(string, MSG_BONUS_BASE + 40);
}

void play_level(char level)
{
	int fire = 0;
	int i;
	ast_hdl_t a=0;
	char string[100];
	
	onoff_t boost=OFF;
	onoff_t shield=OFF;
	rotdir_t rotdir=ROT_NONE;
	
	for (i=0; i<2+level; i++) // Actually: load level data or so
	{
		a = asteroid_create(get_rnd_coord(OBJ_MAX_X), 
				       get_rnd_coord(OBJ_MAX_Y), AST_BIG);
		object_accel(the_asteroids[a].asteroid_obj,
			     rand() % (OBJ_MAX_DXY<<1)-(OBJ_MAX_DXY), 
			     rand() % (OBJ_MAX_DXY<<1)-(OBJ_MAX_DXY));
			  
	}
	
	frame_counter = 0;    
	
	cls();
	sprintf(string, "Wave: %02d", level);
	write_cent(string, 102);
	*JIFFY=0;
	while (*JIFFY<50);

	playscreen_init();

	ship_reset();
	
	while (!quit && noflives!=0 && nof_asteroids>0)
	{
		render_frame(boost, shield, noflives);
		check_controls(&rotdir, &boost, &shield, &fire);
		ship_rotate(rotdir);
		if (boost==ON) ship_accel();
		ship_move(); // en schiet op een beetje!
		asteroids_move();
		bullets_move();
		bullets_n_asteroids();
		ship_shield_set(shield);
		if (object_get_state(the_ship.ship_obj)==DYING)
		{
			ship_destroy();
			boost=OFF;
			ship_init();
		}
		if (ship_hit(shield))
		{
			beep(); // explosion function should be here
			noflives--;
		}	
		if (fire) bullet_fire();
		check_quit(&quit);
	}
	render_frame(boost, shield, noflives); // update noflives on screen
	if (nof_asteroids==0)
	{
		add_bonus(level, noflives);
		sprintf(string,"Wave %d completed!",level);
		write_cent(string, MSG_BASE);

		kilbuf();
		while (!kbhit());
		kilbuf();
	}

}

void main ()
{
	char string[100];

	char *CLICKSW=(char *)0xF3DB;
	char clicksw_old=*CLICKSW;
	*CLICKSW=0;        

	srand(*JIFFY);

	render_init();

	objects_init();
	ship_init();
	bullets_init();
	asteroids_init();
	
	score = 0;
		
	level = 0;
	
	while (!quit && noflives>0)
	{
		play_level(++level);
	}

	if (noflives==0)
	{
		sprintf(string,"Game over! You lost! (Sucker!)");
	}
	else
		sprintf(string,"Chickening out, eh!? Whimp!");
	
	write_cent(string, MSG_BASE);
	
	sprintf(string,"Total score: %d points", score);
	write_cent(string, MSG_BASE+80);

	sprintf(string,"You reached wave %d", level);
	write_cent(string, MSG_BASE+100);

	kilbuf();
	while (!kbhit());
	
	kilbuf();
	screen(0);
	*CLICKSW=clicksw_old;
}
