/* $Id: controls.h,v 1.2 2002/09/27 17:27:26 manuel Exp $
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

extern void check_quit(char *quit);

extern void check_controls(rotdir_t *rotdir, onoff_t *boost, onoff_t *shield, onoff_t *fire);
#endif

