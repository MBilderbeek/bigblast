/* $Id: object.c,v 1.7 2004/01/27 22:26:47 manuel Exp $
 *
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to handle objects. Yup. That's it.
 */

/*
 * INCLUDES
 */

#include "object.h"
#include "math.h"
#include "glib.h"

/*
 * LOCAL DEFINITIONS
 */

#define NOFOBJECTS 128

#ifdef USE_FUNCTIONS
static object_t objects[NOFOBJECTS];
#else
object_t objects[NOFOBJECTS];
#endif

static obj_hdl_t object_last;

extern char mysqrt(int n);

/*
 * EXTERNAL FUNCTIONS
 */

void objects_init (void)
{
	int i;
	object_t *the_object = objects;
	
	for (i = 0; i < NOFOBJECTS; i++)
	{
		the_object->x = 0;
		the_object->y = 0;
		the_object->x_prev[0] = 0;
		the_object->x_prev[1] = 0;
		the_object->y_prev[0] = 0; 
		the_object->y_prev[1] = 0; 
		the_object->dx = 0;
		the_object->dy = 0;
		the_object->size = 0;
		the_object++;
	}

	object_last = 0;
}

obj_hdl_t object_create(int x, int y, char dx, char dy, int size)
{

	int i;
	object_t *the_object = objects;
	
	for (i = 0; i < NOFOBJECTS; i++)
	{	
		if (the_object->size == 0)
		{
			the_object->x = x + OBJ_MAX_X - size;
			the_object->x %= (OBJ_MAX_X - size);
			the_object->y = y + OBJ_MAX_Y - size;
			the_object->y %= (OBJ_MAX_Y - size);
			the_object->x_prev[0] = the_object->x; 
			the_object->x_prev[1] = the_object->x; 
			the_object->y_prev[0] = the_object->y;
			the_object->y_prev[1] = the_object->y;
			the_object->dx = dx;
			the_object->dy = dy;
			the_object->size = size;
			the_object->state = NEW;
			break; // exit for loop
		}
		the_object++;
	}
	
	// assuming that we never run out of object space
	
	return(i);	
}

void object_destroy(obj_hdl_t *object)
{
		objects[*object].size = 0;
		*object = OBJ_VOID;
}

#ifdef USE_FUNCTIONS
int object_get_x(obj_hdl_t object)
{
	return (objects[object].x);
}

int object_get_y(obj_hdl_t object)
{
	return (objects[object].y);
}

void object_set_x(obj_hdl_t object, int x)
{
	objects[object].x = x;
}

void object_set_y(obj_hdl_t object, int y)
{
	objects[object].y = y;
}

int object_get_x_prev(obj_hdl_t object)
{
	return (objects[object].x_prev[c_dpage]);
}

int object_get_y_prev(obj_hdl_t object)
{
	return (objects[object].y_prev[c_dpage]);
}

int object_get_x_prev_op(obj_hdl_t object)
{
	return (objects[object].x_prev[c_apage]);
}

int object_get_y_prev_op(obj_hdl_t object)
{
	return (objects[object].y_prev[c_apage]);
}

int object_get_dx(obj_hdl_t object)
{
	return (objects[object].dx);
}

int object_get_dy(obj_hdl_t object)
{
	return (objects[object].dy);
}

int object_get_size(obj_hdl_t object)
{
	return (objects[object].size);
}

state_e object_get_state(obj_hdl_t object)
{
	return (objects[object].state);
}

void object_set_state(obj_hdl_t object, state_e state)
{
	objects[object].state = state;
}
#endif

void object_move(obj_hdl_t object)
{
	object_t *the_object = &(objects[object]);
	int temp; // wat een kutcompiler zeg!
	int limit;

	the_object->x_prev[c_dpage] = the_object->x;
	the_object->y_prev[c_dpage] = the_object->y;
	
	limit = OBJ_MAX_X - the_object->size;
	temp = the_object->x + the_object->dx;
	if ( temp < 0 ) temp += limit;
	if ( temp >= limit ) temp -= limit;
	the_object->x = temp;

	limit = OBJ_MAX_Y - the_object->size;
	temp = the_object->y + the_object->dy;
	if ( temp < 0 ) temp += limit;
	if ( temp >= limit ) temp -= limit;
	the_object->y = temp;
}

void object_accel(obj_hdl_t object, char ddx, char ddy)
{
	object_t *the_object = &(objects[object]);
	char l;
	char dx, dy;
	
	dx = the_object->dx + ddx;
	dy = the_object->dy + ddy;
	
	l = mysqrt (dx*dx + dy*dy);

	if (l > OBJ_MAX_DXY)
	{
		dx = (dx*OBJ_MAX_DXY)/l;
		dy = (dy*OBJ_MAX_DXY)/l;
	}
	
	the_object->dx = dx;
	the_object->dy = dy;
}

char mysqrt(int n)
{
	int r, rn=n>>1;

	do
	{	r=rn;
		rn=(r + n/r)>>1;
	}
	while ((r-rn)>1);
	return (char) rn;
}
	
	
