/* $Id: controls.h,v 1.4 2003/02/14 17:52:28 eric Exp $
 *
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

extern char check_quit();

extern void check_controls(rotdir_t *rotdir, onoff_t *boost, onoff_t *shield, onoff_t *fire);

extern char keypressed();

#endif

