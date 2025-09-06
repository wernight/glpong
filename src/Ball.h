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

#include <memory>

#include "IObject.h"
#include "Board.h"
#include "Paddle.h"
#include "Vector.h"
#include "RandomMT.h"

// Structures
struct Particle            // Create A Structure For Particle
{
//  bool  active;          // Active (Yes/No)
  float  life;          // life
  float  fade;          // Fade Speed
  float  x;            // X Position
  float  y;            // Y Position
  float  z;            // Z Position
  float  color[4];        // RGBA Color - A = life
};                  // Particles Structure

class Ball : public IObject
{
// Constructor
public:
  Ball(std::shared_ptr<Board> board, std::shared_ptr<Paddle> left_paddle, std::shared_ptr<Paddle> right_paddle, GLuint texture);
  virtual ~Ball();

// Implementation of IObject.
  // Update the object.
  void Update(float fTime) override;

  // Render the object.
  void Render() const override;

  // Process event.
  bool ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam) override;

// Attributes
  // Current ball's location.
  inline Vector2D GetPosition()
  {
    return m_vBallPosition;
  }

  // Current ball's velocity.
  inline Vector2D GetSpeed()
  {
    return m_vBallSpeed;
  }

// Implementation
private:
  // Create a new ball aimed toward left or right player.
  void NewBall(bool bGoToLeft);

  // Detects if the ball collide with a point A.
  inline bool HitPoint(Vector2D &vOldPos, Vector2D &vNewPos, Vector2D &vSpeed, Vector2D &ptA);

  std::shared_ptr<Board> board_;
  std::shared_ptr<Paddle> left_paddle_;
  std::shared_ptr<Paddle> right_paddle_;
  GLuint texture_;
  Vector2D m_vBallPosition;  // Ball's position.
  Vector2D m_vBallSpeed;    // Ball's speed.
  Particle  m_particles[50];  // Particles
  GLuint m_nList;
  RandomMT m_rand;
};
