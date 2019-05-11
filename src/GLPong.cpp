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
 *
 * GLPong main implementation file.
 *
 * @name GLPong
 * @version 2.0
 * @date 2004.03.07
 */

/**
 * Copyright (c) 2003 Werner BEROUX
 * Mail: werner@beroux.com
 * Web : www.beroux.com
 */

// GLPong.cpp : Defines the entry point for the application.
//

#include "StdAfx.h"
#include "GLPong.h"
#include "Firework.h"
#include "SceneManager.h"
#include "Board.h"
#include "Paddle.h"
#include "AiPaddle.h"
#include "Ball.h"

#define ROCKETS				4		// Number of simultaneous rockets for the firework
#define SCREEN_FREQUENCY	60		// 60Hz

// Global Variables:
CSceneManager	g_scene;
CBoard		g_board;
CPaddle		*g_ppaddleLeft;
CPaddle		g_paddleRight(false);
CBall		g_ball;
SDL_Surface *g_sdlSurface;				// This is our SDL surface
GLuint		g_texture[NUM_TEXTURES];	// Storage For Our Particle Texture

int main(int argc, char *argv[])
{
	const SDL_VideoInfo *videoInfo;	// this holds some info about our display
	char		szBuffer[5];
	SDL_Event	event;				// used to collect events
	int			videoFlags;			// Flags to pass to SDL_SetVideoMode
	int			done = false;		// main loop variable
	int			isActive = true;	// whether or not the window is active
	Uint32		nCurTicks,
				nPrevTicks,
				nLastDraw;
	float		t;

	// initialize SDL
#ifdef _DEBUG
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
#else
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
#endif
	{
		std::cerr << "Video initialization failed: " << SDL_GetError() << std::endl;
		return 1;
	}

	// Fetch the video info
	if (!(videoInfo = SDL_GetVideoInfo()))
	{
		std::cerr << "Video query failed: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// the flags to pass to SDL_SetVideoMode
	videoFlags  = SDL_OPENGL;			// Enable OpenGL in SDL
	videoFlags |= SDL_GL_DOUBLEBUFFER;	// Enable double buffering
	videoFlags |= SDL_HWPALETTE;		// Store the palette in hardware
	videoFlags |= SDL_RESIZABLE;		// Enable window resizing

	// Full-screen?
	printf("Full-screen mode [Y/N]?");
#ifdef _DEBUG
	strcpy(szBuffer, "no");
#else
	scanf("%5s", szBuffer);
#endif
	if (strlen(szBuffer) > 0 && tolower(szBuffer[0]) == 'y')
	{
		videoFlags |= SDL_FULLSCREEN;		// Enable full-screen mode
		SDL_ShowCursor(SDL_DISABLE);
	}

	// Versus AI?
	printf("Play against AI [Y/N]?");
#ifdef _DEBUG
	strcpy(szBuffer, "yes");
#else
	scanf("%5s", szBuffer);
#endif
	if (strlen(szBuffer) > 0 && tolower(szBuffer[0]) == 'y')
	{
		// AI.
		g_ppaddleLeft = new CAiPaddle(true);
		((CAiPaddle*)g_ppaddleLeft)->Create(&g_ball);
	}
	else
	{
		// Humain.
		g_ppaddleLeft = new CPaddle(true);
	}
	g_ball.Create(&g_board, g_ppaddleLeft, &g_paddleRight);

	// This checks to see if surfaces can be stored in memory
	if (videoInfo->hw_available)
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;

	// This checks if hardware blits can be done
	if (videoInfo->blit_hw)
		videoFlags |= SDL_HWACCEL;

	// Sets up OpenGL double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Get a SDL surface
	if (!(g_sdlSurface = SDL_SetVideoMode(640, 480, 16, videoFlags)))
	{
		std::cerr << "Video mode set failed: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// Initialize our window.
	InitGL();

	// resize the initial window
	ReSizeGLScene(640, 480);

	// Main loop
	nLastDraw = nPrevTicks = SDL_GetTicks();
	while (!done)
	{
		// Handle the events in the queue
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_VIDEORESIZE:
				// handle resize event
				ReSizeGLScene(event.resize.w, event.resize.h);
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					SDL_Quit();
					return 0;
				case SDLK_F1:
					SDL_WM_ToggleFullScreen(g_sdlSurface);
					break;
				case SDLK_PAUSE:
					isActive = !isActive;
					break;
				default:
					g_scene.ProcessEvent(IObject::eventKeyDown, event.key.keysym.sym, 0);
				}
				break;

			case SDL_KEYUP:
				g_scene.ProcessEvent(IObject::eventKeyUp, 0, event.key.keysym.sym);
				break;

			case SDL_QUIT:
				done = true;
				break;
			}
		}

		// Draw the scene
		if (isActive)
		{
			// Game logic update
			nCurTicks = SDL_GetTicks();
			t = (nCurTicks - nPrevTicks) / 1000.0f;
			nPrevTicks = nCurTicks;
			if (t > 0.3f)
				t = 0.0f;
			UpdateScene(t);

			// Render scene
			if ((nCurTicks - nLastDraw) > 1000/SCREEN_FREQUENCY)
			{
				nLastDraw = nCurTicks;
				DrawGLScene();
				SDL_GL_SwapBuffers();
			}

			// Gather our frames per second
			DrawFPS();

			SDL_Delay(2);
		}
		else
			SDL_Delay(100);
	}

	// End
	SDL_Quit();
	delete g_ppaddleLeft;
	return EXIT_SUCCESS;
}

void DrawFPS()
{
	static int	nFrames = 0;
	static Uint32 nStartTime = SDL_GetTicks();
	Uint32		nCurrentTime = SDL_GetTicks();

	nFrames++;
	if (nCurrentTime - nStartTime >= 5000)
	{
		GLfloat fSeconds = (nCurrentTime - nStartTime) / 1000.0f;
		GLfloat fFPS = nFrames / fSeconds;
		printf("%d frames in %g seconds = %g FPS\n", nFrames, fSeconds, fFPS);
		nStartTime = nCurrentTime;
		nFrames = 0;
	}
}

void UpdateScene(float t)
{
	// Scene manager
	g_scene.Update(t);
}

void DrawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					// Clear The Screen And The Depth Buffer
	glLoadIdentity();													// Reset The Modelview Matrix

	// Loocking to...
	// 3D Look.
	gluLookAt(	0,	-120,	-100,
				0,	0,		0,
				0,	1,		0);
/*/
	// 2D Look.
	gluLookAt(	0,	17,		-180,
				0,	17,		0,
				0,	1,		0);
//*/

	// Scene manager
	g_scene.Render();
}

int InitGL(GLvoid)														// All Setup For OpenGL Goes Here
{
	GLfloat LightAmbient[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightDiffuse[]=		{ 1.0f, 1.0f, 1.0f, 1.0f };

	if (!LoadGLTextures())								// Jump To Texture Loading Routine
		return false;									// If Texture Didn't Load Return false

	glShadeModel(GL_SMOOTH);											// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);								// Black Background
	glClearDepth(1.0f);													// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	// Lumières
	GLfloat LightPosition[]=	{ 30.0f, 50.0f, -100.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);					// Position The Light

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);						// Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);						// Setup The Diffuse Light
	glEnable(GL_LIGHT1);												// Enable Light One
	glEnable(GL_LIGHTING);												// Enable Light One
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glBlendFunc(GL_ONE, GL_ONE);

	// Scene manager.
	if (!g_scene.AddObject(&g_board) ||
		!g_scene.AddObject(g_ppaddleLeft) ||
		!g_scene.AddObject(&g_paddleRight) ||
		!g_scene.AddObject(&g_ball) ||
		!g_scene.Initialize())
		return false;
	return true;
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)						// Resize And Initialize The GL Window
{
	if (height == 0)													// Prevent A Divide By Zero By
		height = 1;														// Making Height Equal One

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity();													// Reset The Projection Matrix
	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 1, 1000);

	glMatrixMode(GL_MODELVIEW);											// Select The Modelview Matrix
	glLoadIdentity();													// Reset The Modelview Matrix
}

int LoadGLTextures()											// Load Bitmaps And Convert To Textures
{
	int Status = false;											// Status Indicator
	int	i;

	// Create storage space for the texture
	SDL_Surface *TextureImage[NUM_TEXTURES];

	// Load The Bitmap, Check For Errors.
	if ((TextureImage[0] = SDL_LoadBMP("Particle.bmp")) &&
		(TextureImage[1] = SDL_LoadBMP("Small Blur Star.bmp")))
	{
		Status = true;											// Set The Status To true

		glGenTextures(NUM_TEXTURES, &g_texture[0]);				// Create The Texture ( CHANGE )

		for (i=0; i<NUM_TEXTURES; i++)							// Loop Through Both Textures
		{
			// Typical Texture Generation Using Data From The TGA ( CHANGE )
			glBindTexture(GL_TEXTURE_2D, g_texture[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->w, TextureImage[0]->h, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->pixels);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		}
	}

	// Free up any memory we may have used
	for (i=0; i<NUM_TEXTURES && TextureImage[i]; i++)
		SDL_FreeSurface(TextureImage[i]);

	return Status;												// Return The Status
}

void DrawFirework()
{
	CFireworkRocket	rockets[ROCKETS];						// Firework
	SDL_Event	event;
	bool		done=false;								// Bool Variable To Exit Loop
	Uint32		nStartTime;
	int 		nCurTicks, nPrevTicks;
	float		dt;
	int 		i;

	// Init GL
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glBlendFunc(GL_ONE, GL_ONE);						// Type Of Blending To Perform
	glBindTexture(GL_TEXTURE_2D, g_texture[1]);			// Select Our Texture

	// Create a rocket
	for (i=0; i<ROCKETS; ++i)
		if (!rockets[i].Create())
			return;

	// Start timer
	nStartTime = SDL_GetTicks();

	// Message loop
	nPrevTicks = SDL_GetTicks();
	while(!done)									// Loop That Runs While done=false
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_VIDEORESIZE:
				// handle resize event
				ReSizeGLScene(event.resize.w, event.resize.h);
				break;

			case SDL_KEYDOWN:
				if (event.key.keysym.sym != SDLK_ESCAPE)
					break;
			case SDL_QUIT:
				done = true;
				break;
			}
		}

		// Update the scene
		nCurTicks = SDL_GetTicks();
		dt = (nCurTicks - nPrevTicks) / 1000.0f;
		nPrevTicks = nCurTicks;
		if (dt > 0.3f)
			dt = 0.0f;
		for (i=0; i<ROCKETS; ++i)
			rockets[i].Update(dt);

		// Draw scene
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
		glLoadIdentity();										// Reset The ModelView Matrix

		// Draw board
		// 3D Look.
		gluLookAt(	0,	-120,	-100,
					0,	0,		0,
					0,	1,		0);

		g_board.Render();
		g_paddleRight.Render();
		g_ppaddleLeft->Render();

		// Draw fireword
		glLoadIdentity();
		glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glEnable(GL_TEXTURE_2D);
			glTranslatef(0.0f, 0.0f, -40.0f);
			for (i=0; i<ROCKETS; ++i)
				rockets[i].Render();
		glPopAttrib();

		// Swap Buffers (Double Buffering)
		glFlush();
		SDL_GL_SwapBuffers();
	}
	glPopAttrib();
}

