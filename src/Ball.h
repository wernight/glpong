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

#ifndef INC_BALL_H_
#define INC_BALL_H_

#ifdef _WIN32
#pragma once
#endif

#include "IObject.h"
#include "Board.h"
#include "Paddle.h"
#include "Vector.h"
#include "RandomMT.h"

// Structures
struct PARTICLE            // Create A Structure For Particle
{
//  bool  active;          // Active (Yes/No)
  float  life;          // life
  float  fade;          // Fade Speed
  float  x;            // X Position
  float  y;            // Y Position
  float  z;            // Z Position
  float  color[4];        // RGBA Color - A = life
};                  // Particles Structure

class CBall : public IObject
{
// Constructor
public:
  CBall();
  virtual ~CBall();

  // Create the ball.
  bool Create(CBoard *pBoard, CPaddle *pLeftPaddle, CPaddle *pRightPaddle);

// Implementation of IObject.
  // Initialize the object.
  bool Initialize();

  // Update the object.
  void Update(float fTime);

  // Render the object.
  void Render() const;

  // Process event.
  bool ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam);

// Attributes
  // Current ball's location.
  inline CVector2D GetPosition()
  {
    return m_vBallPosition;
  }

  // Current ball's velocity.
  inline CVector2D GetSpeed()
  {
    return m_vBallSpeed;
  }

// Implementation
private:
  // Create a new ball aimed toward left or right player.
  void NewBall(bool bGoToLeft);

  // Detects if the ball collide with a point A.
  inline bool HitPoint(CVector2D &vOldPos, CVector2D &vNewPos, CVector2D &vSpeed, CVector2D &ptA);

  CVector2D  m_vBallPosition,  // Ball's position.
        m_vBallSpeed;    // Ball's speed.
  PARTICLE  m_particles[50];  // Particles
  CBoard    *m_pBoard;
  CPaddle    *m_pLeftPaddle,
        *m_pRightPaddle;
  GLuint    m_nList;
  CRandomMT  m_rand;
};

#endif

