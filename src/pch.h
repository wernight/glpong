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

#pragma once

#include <GL/glew.h>

#ifdef WIN32                          // If We're Under MSVC
#pragma comment(lib, "OpenGL32.lib")  // We're Telling The Linker To Look For The OpenGL32.lib
#pragma comment(lib, "GLu32.lib")     // The GLu32.lib Library...
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2.lib")
#endif

#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#ifndef DWORD
typedef unsigned long DWORD;
#endif

// Constants
#ifndef M_PI
#define M_PI 3.1415928
#endif
