/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * Copyright (c) 2003 Werner BEROUX
 * Mail: werner@beroux.com
 * Web : www.beroux.com
 */

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __STDAFX_H__
#define __STDAFX_H__

#ifdef WIN32											// If We're Under MSVC
#pragma comment(lib, "OpenGL32.lib")					// We're Telling The Linker To Look For The OpenGL32.lib
#pragma comment(lib, "GLu32.lib")						// The GLu32.lib Library...
#pragma comment(lib, "./lib/SDL/lib/SDLmain.lib")		// The SDLmain.lib And
#pragma comment(lib, "./lib/SDL/lib/SDL.lib")			// The SDL.lib Libraries
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <GL/glu.h>

//#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <iostream>

#ifndef DWORD
typedef unsigned long DWORD;
#endif

// Constants
#ifndef M_PI
#define M_PI 3.1415928
#endif

#ifdef _DEBUG
# define ASSERT(f)			assert(f)
# define VERIFY(f)			ASSERT(f)
#else
# define ASSERT(f)			((void)0)
# define VERIFY(f)			((void)(f))
#endif

#endif
