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
#include "GLPong.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <SDL2/SDL.h>

#include <filesystem>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>

#include "Paddle.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

constexpr int kScreenFrequency = 60;  // 60Hz

static std::filesystem::path GetResourcePath(const std::string& relative) {
  const char* appdir = std::getenv("APPDIR");
  if (!appdir) {
    // Not running in an AppImage -> fallback to current path
    return std::filesystem::current_path() / relative;
  }
  return std::filesystem::path(appdir) / relative;
}

static bool UserInputBoolean() {
  std::string input;
  std::cin >> input;
  return !input.empty() && tolower(input[0]) == 'y';
}

// Load Bitmaps And Convert To Textures
static GLuint LoadGLTextures(const char* filename) {
  int width, height, channels;
  unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
  if (!data) {
    std::cerr << "Failed to load image: " << filename << "\n";
    return 0;
  }

  GLenum format;
  if (channels == 1)
    format = GL_LUMINANCE;
  else if (channels == 3)
    format = GL_RGB;
  else if (channels == 4)
    format = GL_RGBA;
  else {
    stbi_image_free(data);
    throw std::runtime_error("Unsupported channel count");
  }

  GLuint gl_texture;
  glGenTextures(1, &gl_texture);

  // Typical Texture Generation Using Data From The TGA ( CHANGE )
  glBindTexture(GL_TEXTURE_2D, gl_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);

  return gl_texture;
}

GLPong::GLPong() {
// initialize SDL
#ifdef _DEBUG
  Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;
#else
  Uint32 flags = SDL_INIT_VIDEO;
#endif
  if (SDL_Init(flags) < 0) {
    std::cerr << "Video initialization failed: " << SDL_GetError() << std::endl;
    throw std::runtime_error("Video initialization failed");
  }

  // the flags to pass to SDL_SetVideoMode
  Uint32 videoFlags;                   // Flags to pass to SDL_SetVideoMode
  videoFlags = SDL_WINDOW_OPENGL;      // Enable OpenGL in SDL
  videoFlags |= SDL_WINDOW_RESIZABLE;  // Enable window resizing

  // Full-screen?
#ifndef __EMSCRIPTEN__
  std::cout << "Full-screen mode [Y/N]?";
  if (UserInputBoolean()) {
    videoFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;  // Enable full-screen mode
    SDL_ShowCursor(SDL_DISABLE);
  }
#endif

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  // Sets up OpenGL double buffering
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

  constexpr int kWidth = 800;
  constexpr int kHeight = 600;

  // Get a SDL surface
  sdl_window_ = SDL_CreateWindow("GLPong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, kWidth,
                                 kHeight, videoFlags);
  if (!sdl_window_) {
    std::cerr << "Video mode set failed: " << SDL_GetError() << std::endl;
    throw std::runtime_error("Video mode set failed");
  }

  gl_context_ = SDL_GL_CreateContext(sdl_window_);
  if (!gl_context_) {
    std::cerr << "GL context creation failed: " << SDL_GetError() << std::endl;
    throw std::runtime_error("Video mode set failed");
  }

  // Initialize GLEW.
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    throw std::runtime_error("GLEW initialization failed");
  }

  // Initialize our window.
  InitGL();

  // resize the initial window
  SDL_SetWindowSize(sdl_window_, kWidth, kHeight);
  glViewport(0, 0, kWidth, kHeight);
}

GLPong::~GLPong() {
  if (gl_context_) SDL_GL_DeleteContext(gl_context_);
  if (sdl_window_ != nullptr) SDL_DestroyWindow(sdl_window_);
  SDL_Quit();
}

void GLPong::ProcessEvents() {
  // Handle the events in the queue
  static SDL_Event sdl_event;
  while (SDL_PollEvent(&sdl_event)) {
    switch (sdl_event.type) {
      case SDL_WINDOWEVENT:
        if (sdl_event.window.event == SDL_WINDOWEVENT_RESIZED)
          glViewport(0, 0, sdl_event.window.data1, sdl_event.window.data2);
        break;

      case SDL_KEYDOWN:
        switch (sdl_event.key.keysym.sym) {
#ifndef __EMSCRIPTEN__
          case SDLK_ESCAPE:
            SDL_Quit();
            game_is_still_running_ = false;
            return;
#endif
          case SDLK_F1: {
            Uint32 flags = SDL_GetWindowFlags(sdl_window_);
            if (flags & SDL_WINDOW_FULLSCREEN)
              SDL_SetWindowFullscreen(sdl_window_, 0);  // back to windowed
            else
              SDL_SetWindowFullscreen(sdl_window_, SDL_WINDOW_FULLSCREEN);
          } break;
          case SDLK_PAUSE:
            is_active_ = !is_active_;
            break;
        }
        break;
    }

    scene_.ProcessEvent(sdl_event);
  }
}

void GLPong::Draw() {
  ProcessEvents();
  if (!is_active_) return;

  // Game logic update
  Uint32 cur_ticks = SDL_GetTicks();
  float dt = (cur_ticks - prev_ticks_) / 1000.0f;
  prev_ticks_ = cur_ticks;
  if (dt > 0.3f) dt = 0.0f;

  scene_.Update(dt);

  if (firework_) {
    if (firework_->IsDone()) {
      // Once the firework is done, we remove it and reset the game.
      scene_.RemoveObject(firework_);
      firework_.reset();
      board_->Reset();
      scene_.AddObject(ball_);
    }
  } else if (board_->IsGameOver()) {
    firework_ = std::make_shared<Firework>(star_texture_);
    scene_.AddObject(firework_);
    // We avoid to render the ball during the firework.
    scene_.RemoveObject(ball_);
  }

  // Render scene
  if (cur_ticks - last_draw_ticks_ > 1000 / kScreenFrequency) {
    last_draw_ticks_ = cur_ticks;
    DrawGLScene();
    SDL_GL_SwapWindow(sdl_window_);
  }

  // Gather our frames per second
  DrawFPS();
}

bool GLPong::Run() {
  // Main loop
  last_draw_ticks_ = prev_ticks_ = SDL_GetTicks();
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg([](void* arg) { static_cast<GLPong*>(arg)->Draw(); }, this,
                               /*fps=*/0, /*simulate_infinite_loop=*/1);
#else
  while (game_is_still_running_) {
    Draw();

    if (is_active_)
      SDL_Delay(2);
    else
      SDL_Delay(100);
  }
#endif

  // End
  return true;
}

void GLPong::DrawFPS() {
  static int frames = 0;
  static Uint32 start_ticks = SDL_GetTicks();
  Uint32 current_ticks = SDL_GetTicks();

  frames++;
  if (current_ticks - start_ticks >= 5000) {
    GLfloat seconds = (current_ticks - start_ticks) / 1000.0f;
    GLfloat fps = frames / seconds;
    std::cout << frames << " frames in " << seconds << " seconds = " << fps << " FPS" << std::endl;
    start_ticks = current_ticks;
    frames = 0;
  }
}

void GLPong::DrawGLScene() {
  // Clear the screen and the depth buffer.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 model = glm::mat4(1.0f);

  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, -120.0f, -100.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 1.0f, 0.0f));

  glm::mat4 projection = glm::perspective(glm::radians(45.0f),  // field of view in radians
                                          4.0f / 3.0f,          // width/height of viewport
                                          0.1f,                 // near plane
                                          1000.0f);             // far plane

  // Scene manager
  scene_.Render(model, view, projection);
}

// All Setup For OpenGL Goes Here
void GLPong::InitGL() {
  particle_texture_ = LoadGLTextures(GetResourcePath("particle.png").c_str());
  star_texture_ = LoadGLTextures(GetResourcePath("small_blur_star.png").c_str());

  auto paddle_left = std::make_shared<Paddle>(true);
  auto paddle_right = std::make_shared<Paddle>(false);
  board_ = std::make_shared<Board>();
  ball_ = std::make_shared<Ball>(board_, paddle_left, paddle_right, particle_texture_);
  paddle_left->TrackBall(ball_);
  paddle_right->TrackBall(ball_);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Black Background
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  // Scene manager.
  scene_.AddObject(board_);
  scene_.AddObject(paddle_left);
  scene_.AddObject(paddle_right);
  scene_.AddObject(ball_);
}
