/* $Id: object.h,v 1.9 2004/08/22 20:01:27 eric Exp $
 *
 * AUTHOR(S)   : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains typedefs of objects. That's all folks!
 *
 */

#ifndef OBJECT_H
#define OBJECT_H

/*
 * DEFINITIONS
 */

/* Using a big object space and scale that down for rendering
 * gives smoother movements
 */

#define OBJ_GFX_FACTOR 3
#define OBJ_MAX_X      (256 << OBJ_GFX_FACTOR)
#define OBJ_MAX_Y      (204 << OBJ_GFX_FACTOR)

#define OBJ2GFX(c)     ((c) >> OBJ_GFX_FACTOR)
#define GFX2OBJ(c)     ((c) << OBJ_GFX_FACTOR)

#define OBJ_MAX_DXY    31

#define OBJ_VOID       -1


/*
 * TYPE DEFINITIONS
 */

typedef char obj_hdl_t;

typedef enum
{
	NEW,
	ALIVE,
	DYING,
	DEAD
} state_e;

typedef struct
{
	int x;
	int y;
	int x_prev[2];
	int y_prev[2];
	char dx;
	char dy;
	int size;
	state_e state;
} object_t;

/*
 * INTERFACE FUNCTIONS
 */

extern void objects_init (void);
extern obj_hdl_t object_create(int x, int y, char dx, char dy, int size);
extern void object_destroy(obj_hdl_t *object);

#ifdef USE_FUNCTIONS
	extern int object_get_x(obj_hdl_t object);
	extern int object_get_y(obj_hdl_t object);
	extern int object_get_x_prev(obj_hdl_t object);
	extern int object_get_y_prev(obj_hdl_t object);
	extern int object_get_x_prev_op(obj_hdl_t object);
	extern int object_get_y_prev_op(obj_hdl_t object);
	extern int object_get_dx(obj_hdl_t object);
	extern int object_get_dy(obj_hdl_t object);
	extern int object_get_size(obj_hdl_t object);
	extern state_e object_get_state(obj_hdl_t object);

	extern void object_set_x(obj_hdl_t object, int x);
	extern void object_set_y(obj_hdl_t object, int y);
	extern void object_set_state(obj_hdl_t object, state_e state);
#else
	extern object_t objects[];

	#define object_get_x(obj)          (objects[obj].x)
	#define object_get_y(obj)          (objects[obj].y)
	#define object_get_x_prev(obj)     (objects[obj].x_prev[c_dpage])
	#define object_get_y_prev(obj)     (objects[obj].y_prev[c_dpage])
	#define object_get_x_prev_op(obj)  (objects[obj].x_prev[c_apage])
	#define object_get_y_prev_op(obj)  (objects[obj].y_prev[c_apage])
	#define object_get_dx(obj)         (objects[obj].dx)
	#define object_get_dy(obj)         (objects[obj].dy)
	#define object_get_size(obj)       (objects[obj].size)
	#define object_get_state(obj)      (objects[obj].state)
	
	#define object_set_x(obj,X)        (objects[obj].x=(X))
	#define object_set_y(obj,Y)        (objects[obj].y=(Y))
//	#define object_set_x_prev(obj,X)   (objects[obj].x_prev=(X))
//	#define object_set_y_prev(obj,Y)   (objects[obj].y_prev=(Y))
//	#define object_set_dx(obj,X)       (objects[obj].dx=(X))
//	#define object_set_dy(obj,Y)       (objects[obj].dy=(Y))
//	#define object_set_size(obj,S)     (objects[obj].size=(S))
	#define object_set_state(obj,S)    (objects[obj].state=(S))
#endif
extern void object_move(obj_hdl_t object);

extern void object_accel(obj_hdl_t object, char ddx, char ddy);

extern char mysqrt(int n);

#endif

