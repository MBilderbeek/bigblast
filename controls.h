/*
 * FILE        : controls.h
 * LAST UPDATE : 2 Jun 2002
 * AUTHOR      : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   contains functions to control controls. Yup. That's it.
 */

/*
 * INCLUDES
 */

#ifndef CONTROLS_H
#define CONTROLS_H
#include "ship.h"

/*
 * EXTERNAL FUNCTIONS
 */

extern void check_quit(char *quit);

extern void check_controls(rotdir_t *rotdir, onoff_t *boost, onoff_t *shield, onoff_t *fire);
#endif

