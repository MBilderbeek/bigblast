/*
 * FILE        : ship.c
 * LAST UPDATE : 10 May 2002
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to handle the ship. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include "object.h"
#include "ship.h"

/*
 * LOCAL DEFINITIONS
 */

ship_t the_ship;

heading_table_t heading2dxdy =
{ 
        {  16,   0 } ,
        {  15,   3 } ,
        {  14,   6 } ,
        {  13,   8 } ,
        {  11,  11 } ,
        {   8,  13 } ,
        {   6,  14 } ,
        {   3,  15 } ,
        {   0,  16 } ,
        {  -3,  15 } ,
        {  -6,  14 } ,
        {  -8,  13 } ,
        { -11,  11 } ,
        { -13,   8 } ,
        { -14,   6 } ,
        { -15,   3 } ,
        { -16,   0 } ,
        { -15,  -3 } ,
        { -14,  -6 } ,
        { -13,  -8 } ,
        { -11, -11 } ,
        {  -8, -13 } ,
        {  -6, -14 } ,
        {  -3, -15 } ,
        {   0, -16 } ,
        {   3, -15 } ,
        {   6, -14 } ,
        {   8, -13 } ,
        {  11, -11 } ,
        {  13,  -8 } ,
        {  14,  -6 } ,
        {  15,  -3 } 
};


/*
 * EXTERNAL FUNCTIONS
 */

void ship_init()
{
	the_ship.ship_obj = object_create (OBJ_MAX_X / 2, OBJ_MAX_Y / 2, 
				0, 0, OBJ_SHIP);
	the_ship.heading = 3 * MAX_HEADING / 4;
	the_ship.heading_prev = -1;
	the_ship.shield_energy = MAX_SHIELD_ENERGY;
	the_ship.shield_state = OFF;
}

void ship_destroy()
{
	object_destroy(&(the_ship.ship_obj));
}

void ship_rotate(rotdir_t direction)
{
	the_ship.heading_prev = the_ship.heading;
	the_ship.heading += (MAX_HEADING + direction);
	the_ship.heading %= MAX_HEADING;
}

void ship_shield_set(onoff_t state)
{
	if (state == ON && the_ship.shield_energy > 0)
		the_ship.shield_state = ON;
	else
		the_ship.shield_state = OFF;
}

void ship_move()
{
	object_move(the_ship.ship_obj);
}

void ship_accel()
{
	char ddx, ddy;
	
	ddx = heading2dxdy[the_ship.heading].x;
	ddy = heading2dxdy[the_ship.heading].y;
	
	object_accel(the_ship.ship_obj, ddx>>1, ddy>>1);
}
