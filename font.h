/* $Id: font.h,v 1.1 2002/12/26 23:54:11 manuel Exp $
 *
 * AUTHOR(S)   : M. Bilderbeek & E. Boon
 *
 * DESCRIPTION :
 *   interface definition for the font functions.
 */

#ifndef _FONT_H
#define _FONT_H

/*
 * INCLUDES
 */


/*
 * DEFINITIONS
 */

#define FONT_W 6
#define FONT_H 8

void write(char *string, unsigned int x, unsigned int  y);
void write_cent(char *string, unsigned int  y);

#endif

