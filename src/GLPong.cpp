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

#include <array>
#include <filesystem>

#include "Firework.h"
#include "SceneManager.h"
#include "Board.h"
#include "Paddle.h"
#include "AiPaddle.h"
#include "Ball.h"

#define ROCKETS        4    // Number of simultaneous rockets for the firework
#define SCREEN_FREQUENCY  60    // 60Hz

// Global Variables:
CSceneManager g_scene;
CBoard g_board;
CPaddle *g_ppaddleLeft;
CPaddle g_paddleRight(false);
CBall g_ball;
SDL_Window* g_pWindow;
SDL_GLContext g_GLContext;
std::array<GLuint, 2> g_textures;  // Storage For Our Particle Texture
int g_done = false;    // main loop variable
int g_isActive = true;  // whether or not the window is active
Uint32 g_nPrevTicks;
Uint32 g_nLastDraw;

std::filesystem::path GetResourcePath(const std::string& relative) {
    const char* appdir = std::getenv("APPDIR");
    if (!appdir) {
        // Not running in an AppImage -> fallback to current path
        return std::filesystem::current_path() / relative;
    }
    return std::filesystem::path(appdir) / relative;
}

bool UserInputBoolean()
{
  std::string input;
  std::cin >> input;
  return !input.empty() && tolower(input[0]) == 'y';
}

void Draw()
{
  // Handle the events in the queue
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_WINDOWEVENT:
      if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        ReSizeGLScene(event.window.data1, event.window.data2);
      break;

    case SDL_KEYDOWN:
      switch (event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
        SDL_Quit();
        g_done = true;
        return;
      case SDLK_F1:
        {
          Uint32 flags = SDL_GetWindowFlags(g_pWindow);
          if (flags & SDL_WINDOW_FULLSCREEN)
               SDL_SetWindowFullscreen(g_pWindow, 0);  // back to windowed
          else
               SDL_SetWindowFullscreen(g_pWindow, SDL_WINDOW_FULLSCREEN);
        }
        break;
      case SDLK_PAUSE:
        g_isActive = !g_isActive;
        break;
      default:
        g_scene.ProcessEvent(IObject::eventKeyDown, event.key.keysym.sym, 0);
      }
      break;

    case SDL_KEYUP:
      g_scene.ProcessEvent(IObject::eventKeyUp, 0, event.key.keysym.sym);
      break;

    case SDL_QUIT:
      g_done = true;
      break;
    }
  }

  // Draw the scene
  if (g_isActive)
  {
    // Game logic update
    Uint32 nCurTicks = SDL_GetTicks();
    float t = (nCurTicks - g_nPrevTicks) / 1000.0f;
    g_nPrevTicks = nCurTicks;
    if (t > 0.3f)
      t = 0.0f;
    UpdateScene(t);

    // Render scene
    if ((nCurTicks - g_nLastDraw) > 1000/SCREEN_FREQUENCY)
    {
      g_nLastDraw = nCurTicks;
      DrawGLScene();
      SDL_GL_SwapWindow(g_pWindow);
    }

    // Gather our frames per second
    DrawFPS();
  }
}

int main(int argc, char *argv[])
{
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
  IMG_Init(IMG_INIT_PNG);

  // the flags to pass to SDL_SetVideoMode
  Uint32 videoFlags;      // Flags to pass to SDL_SetVideoMode
  videoFlags  = SDL_WINDOW_OPENGL;      // Enable OpenGL in SDL
  videoFlags |= SDL_WINDOW_RESIZABLE;   // Enable window resizing

  // Full-screen?
  std::cout << "Full-screen mode [Y/N]?";
  if (UserInputBoolean())
  {
    videoFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;    // Enable full-screen mode
    SDL_ShowCursor(SDL_DISABLE);
  }

  // Versus AI?
  std::cout << "Play against AI [Y/N]?";
  if (UserInputBoolean())
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
  g_ball.Create(&g_board, g_ppaddleLeft, &g_paddleRight, g_textures[0]);

  // Sets up OpenGL double buffering
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

  // Get a SDL surface
  g_pWindow = SDL_CreateWindow("GLPong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, videoFlags);
  if (!g_pWindow)
  {
    std::cerr << "Video mode set failed: " << SDL_GetError() << std::endl;
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  g_GLContext = SDL_GL_CreateContext(g_pWindow);
  if (!g_GLContext)
  {
    std::cerr << "GL context creation failed: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(g_pWindow);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  // Initialize our window.
  InitGL();

  // resize the initial window
  ReSizeGLScene(640, 480);

  // Main loop
  g_nLastDraw = g_nPrevTicks = SDL_GetTicks();
  while (!g_done)
  {
    Draw();

    if (g_isActive)
      SDL_Delay(2);
    else
      SDL_Delay(100);
  }

  // End
  SDL_GL_DeleteContext(g_GLContext);
  SDL_DestroyWindow(g_pWindow);
  IMG_Quit();
  SDL_Quit();
  delete g_ppaddleLeft;
  return EXIT_SUCCESS;
}

void DrawFPS()
{
  static int  nFrames = 0;
  static Uint32 nStartTime = SDL_GetTicks();
  Uint32    nCurrentTime = SDL_GetTicks();

  nFrames++;
  if (nCurrentTime - nStartTime >= 5000)
  {
    GLfloat fSeconds = (nCurrentTime - nStartTime) / 1000.0f;
    GLfloat fFPS = nFrames / fSeconds;
    std::cout << nFrames << " frames in " << fSeconds << " seconds = " << fFPS << " FPS" << std::endl;
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear The Screen And The Depth Buffer
  glLoadIdentity();  // Reset The Modelview Matrix

  // Loocking to...
#if false
  // 3D Look.
  gluLookAt(  0,  -120,  -100,
        0,  0,    0,
        0,  1,    0);
#else
  // 2D Look.
  gluLookAt(  0,  17,    -180,
        0,  17,    0,
        0,  1,    0);
#endif

  // Scene manager
  g_scene.Render();
}

// All Setup For OpenGL Goes Here
int InitGL(GLvoid)
{
  constexpr GLfloat LightAmbient[]=    { 0.5f, 0.5f, 0.5f, 1.0f };
  constexpr GLfloat LightDiffuse[]=    { 1.0f, 1.0f, 1.0f, 1.0f };

  g_textures[0] = LoadGLTextures(GetResourcePath("particle.png").c_str());
  g_textures[1] = LoadGLTextures(GetResourcePath("small_blur_star.png").c_str());

  glShadeModel(GL_SMOOTH);  // Enable Smooth Shading
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Black Background
  glClearDepth(1.0f);  // Depth Buffer Setup
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  // Lumières
  GLfloat LightPosition[]=  { 30.0f, 50.0f, -100.0f, 1.0f };
  glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);  // Position The Light

  glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);  // Setup The Ambient Light
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);  // Setup The Diffuse Light
  glEnable(GL_LIGHT1);  // Enable Light One
  glEnable(GL_LIGHTING);  // Enable Light One
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

// Resize And Initialize The GL Window
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
{
  if (height == 0)  // Prevent A Divide By Zero By
    height = 1;  // Making Height Equal One

  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);  // Select The Projection Matrix
  glLoadIdentity();  // Reset The Projection Matrix
  gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 1, 1000);

  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glLoadIdentity();  // Reset The Modelview Matrix
}

// Load Bitmaps And Convert To Textures
GLuint LoadGLTextures(const char* filename)
{
  // Create storage space for the texture
  SDL_Surface *sdl_surface;

  sdl_surface = IMG_Load(filename);

  // Load The Bitmap, Check For Errors.
  if (sdl_surface == nullptr)
  {
    std::cerr << "Failed to load texture: " << filename << std::endl;
    return 0;
  }

  GLuint gl_texture;
  glGenTextures(1, &gl_texture);

  GLint ncolors = sdl_surface->format->BytesPerPixel;
  GLenum texture_format;
  switch (ncolors)
  {
    case 1:  // Alpha-only
      texture_format = GL_LUMINANCE;
      break;

    case 3:   // R, G, and B channels
      if (sdl_surface->format->Rmask == 0x000000ff)
        texture_format = GL_RGB;
      else
        texture_format = GL_BGR;
      break;

    case 4:  // R, G, B, and A channels
      if (sdl_surface->format->Rmask == 0x000000ff)
        texture_format = GL_RGBA;
      else
        texture_format = GL_BGRA;
      break;

    default:
      // this is not a supported image format
      std::cerr << "Unsupported image format: " << ncolors << std::endl;
      return 0;
  }

  // Typical Texture Generation Using Data From The TGA ( CHANGE )
  glBindTexture(GL_TEXTURE_2D, gl_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, ncolors, sdl_surface->w, sdl_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, sdl_surface->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Free up any memory we may have used
  SDL_FreeSurface(sdl_surface);

  return gl_texture;
}

void DrawFirework()
{
  CFireworkRocket  rockets[ROCKETS];  // Firework
  SDL_Event event;
  bool done=false;  // Bool Variable To Exit Loop
  Uint32 nStartTime;
  int nCurTicks, nPrevTicks;
  float dt;
  int i;

  // Init GL
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glBlendFunc(GL_ONE, GL_ONE);  // Type Of Blending To Perform
  if (g_textures[1] != 0)
    glBindTexture(GL_TEXTURE_2D, g_textures[1]);  // Select Our Texture

  // Create a rocket
  for (i=0; i<ROCKETS; ++i)
    if (!rockets[i].Create())
      return;

  // Start timer
  nStartTime = SDL_GetTicks();

  // Message loop
  nPrevTicks = SDL_GetTicks();
  while(!g_done)  // Loop That Runs While done=false
  {
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
          ReSizeGLScene(event.window.data1, event.window.data2);
        break;

      case SDL_KEYDOWN:
        if (event.key.keysym.sym != SDLK_ESCAPE)
          break;
      case SDL_QUIT:
        g_done = true;
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear Screen And Depth Buffer
    glLoadIdentity();  // Reset The ModelView Matrix

    // Draw board
    // 3D Look.
    gluLookAt(  0,  -120,  -100,
          0,  0,    0,
          0,  1,    0);

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
    SDL_GL_SwapWindow(g_pWindow);
  }
  glPopAttrib();
}

