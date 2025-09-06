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

#pragma once

#include <memory>

#include "Ball.h"
#include "Board.h"
#include "Firework.h"
#include "SceneManager.h"
#include "StdAfx.h"

class GLPong {
 public:
  GLPong();
  ~GLPong();

  bool Run();

 private:
  void ProcessEvents();
  void Draw();
  void DrawFPS();
  void DrawGLScene();
  void InitGL(bool vs_ai);
  void UpdateScene(float t);

  SceneManager scene_;
  std::shared_ptr<Board> board_;
  std::shared_ptr<Firework> firework_;
  std::shared_ptr<Ball> ball_;
  SDL_Window* sdl_window_;
  SDL_GLContext gl_context_;
  GLuint particle_texture_;
  GLuint star_texture_;
  bool game_is_still_running_ = true;  // main loop variable
  bool is_active_ = true;              // whether or not the window is active
  Uint32 prev_ticks_;
  Uint32 last_draw_ticks_;
};
