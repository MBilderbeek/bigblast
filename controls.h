/* $Id: controls.h,v 1.3 2002/12/26 23:54:11 manuel Exp $
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
#endif

