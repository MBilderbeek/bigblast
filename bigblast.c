/*
 * FILE        : bigblast.c
 * LAST UPDATE : 2 Jun 2002
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
//#include "msxbios.h"

int *JIFFY = (int *)0xFC9E;

static int get_rnd_coord(int range)
{
	int netrange = range - GFX2OBJ(SHIP_TILE_SIZE);
	int coord = rand() %(netrange);
	if (coord > (netrange >> 1)) 
		coord += GFX2OBJ(SHIP_TILE_SIZE);
	return(coord);
}

void main ()
{
	char quit=0;
	ast_hdl_t a=0;
	int i=0;
	obj_hdl_t dummy;
	char noflives=10;
	int fire = 0;

	onoff_t boost=OFF;
	onoff_t shield=OFF;
	rotdir_t rotdir=ROT_NONE;

	char *CLICKSW=(char *)0xF3DB;
	char clicksw_old=*CLICKSW;
	*CLICKSW=0;        

	srand(*JIFFY);

	render_init();

	objects_init();
	ship_init();
	bullets_init();
	asteroids_init();
	
	for (i=0; i<3; i++)
	{
		a = asteroid_create(get_rnd_coord(OBJ_MAX_X), 
				       get_rnd_coord(OBJ_MAX_Y), AST_BIG);
		object_accel(the_asteroids[a].asteroid_obj,
			     rand() % (OBJ_MAX_DXY)-(OBJ_MAX_DXY>>1), 
			     rand() % (OBJ_MAX_DXY)-(OBJ_MAX_DXY>>1));
			  
	}
	
	while (!quit && noflives!=0)
	{
		render_frame(boost);
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
		if (ship_hit(&dummy))
		{
			beep(); // explosion function should be here
			noflives--;
		}	
		if (fire) bullet_fire();
		check_quit(&quit);
	}

	screen(0);
	*CLICKSW=clicksw_old;
}