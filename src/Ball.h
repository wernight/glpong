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
 * \todo Check that gluQuads can be created once and used many time.
 */

/**
 * Copyright (c) 2003 Werner BEROUX
 * Mail: werner@beroux.com
 * Web : www.beroux.com
 */

#pragma once

#include <GL/glew.h>

#include <memory>

#include "Board.h"
#include "IObject.h"
#include "Paddle.h"
#include "RandomMT.h"
#include "Vector.h"

// Create A Structure For Particle
struct Particle {
  float life;      // life
  float fade;      // Fade Speed
  float x;         // X Position
  float y;         // Y Position
  float z;         // Z Position
  float color[4];  // RGBA Color - A = life
};

class Ball : public IObject {
  // Constructor
 public:
  Ball(std::shared_ptr<Board> board, std::shared_ptr<Paddle> left_paddle,
       std::shared_ptr<Paddle> right_paddle, GLuint texture);
  virtual ~Ball();

  // Implementation of IObject.
  // Update the object.
  void Update(float dt) override;

  // Render the object.
  void Render() const override;

  // Process event.
  bool ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam) override;

  // Attributes
  // Current ball's location.
  inline Vector2D GetPosition() { return ball_position_; }

  // Current ball's velocity.
  inline Vector2D GetSpeed() { return ball_speed_; }

  // Implementation
 private:
  // Create a new ball aimed toward left or right player.
  void NewBall(bool go_to_left);

  // Detects if the ball collide with a point A.
  inline bool HitPoint(Vector2D &old_pos, Vector2D &new_pos, Vector2D &speed, Vector2D &a_pos);

  std::shared_ptr<Board> board_;
  std::shared_ptr<Paddle> left_paddle_;
  std::shared_ptr<Paddle> right_paddle_;
  GLuint texture_;
  Vector2D ball_position_;  // Ball's position.
  Vector2D ball_speed_;     // Ball's speed.
  Particle particles_[50];  // Particles
  RandomMT rand_;

  GLuint sphere_vao_ = 0;
  GLuint sphere_vbo_ = 0;
  int sphere_vertex_count_ = 0;

  GLuint particles_vao_ = 0;
  GLuint particles_vbo_ = 0;
};
