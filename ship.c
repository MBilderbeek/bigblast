/* $Id: ship.c,v 1.7 2003/05/29 21:56:46 manuel Exp $
 *
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
#include "explosio.h"

/*
 * LOCAL DEFINITIONS
 */

#define SHIP_SIZE (16 << OBJ_GFX_FACTOR)

ship_t the_ship;

heading_table_t heading2dxdy =
{ 
        {  16,   0 } , /* 0  */
        {  15,   3 } , /* 1  */
        {  14,   6 } , /* 2  */
        {  13,   8 } , /* 3  */
        {  11,  11 } , /* 4  */
        {   8,  13 } , /* 5  */
        {   6,  14 } , /* 6  */
        {   3,  15 } , /* 7  */
        {   0,  16 } , /* 8  */
        {  -3,  15 } , /* 9  */
        {  -6,  14 } , /* 10 */
        {  -8,  13 } , /* 11 */
        { -11,  11 } , /* 12 */
        { -13,   8 } , /* 13 */
        { -14,   6 } , /* 14 */
        { -15,   3 } , /* 15 */
        { -16,   0 } , /* 16 */
        { -15,  -3 } , /* 17 */
        { -14,  -6 } , /* 18 */
        { -13,  -8 } , /* 19 */
        { -11, -11 } , /* 20 */
        {  -8, -13 } , /* 21 */
        {  -6, -14 } , /* 22 */
        {  -3, -15 } , /* 23 */
        {   0, -16 } , /* 24 */
        {   3, -15 } , /* 25 */
        {   6, -14 } , /* 26 */
        {   8, -13 } , /* 27 */
        {  11, -11 } , /* 28 */
        {  13,  -8 } , /* 29 */
        {  14,  -6 } , /* 30 */
        {  15,  -3 }   /* 31 */
};


/*
 * EXTERNAL FUNCTIONS
 */

void ship_init()
{
	the_ship.ship_obj = object_create (
			(OBJ_MAX_X - SHIP_SIZE)>>1,
			(OBJ_MAX_Y - SHIP_SIZE)>>1, 
			0, 0, SHIP_SIZE);
	the_ship.heading = 3 * MAX_HEADING / 4;
	the_ship.heading_prev = -1;
	the_ship.shield_energy = MAX_SHIELD_ENERGY;
	the_ship.shield_state = OFF;
}

void ship_destroy()
{
	// explode! HAHAHAHAAAAA!
	explosion_create(object_get_x(the_ship.ship_obj), 
			 object_get_y(the_ship.ship_obj), 
			 EXP_BIG, object_get_dx(the_ship.ship_obj),
			 object_get_dy(the_ship.ship_obj));
	beep();
	
	object_destroy(&(the_ship.ship_obj));
        
}

void ship_reset()
{
	ship_destroy();
	ship_init();
}

void ship_rotate(rotdir_t direction)
{
	the_ship.heading_prev = the_ship.heading;
	the_ship.heading += (MAX_HEADING + direction);
	the_ship.heading %= MAX_HEADING;
}

void ship_shield_set(onoff_t state)
{
	if (state == ON && the_ship.shield_energy-- > 0)
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
