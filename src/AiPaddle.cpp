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

#include "StdAfx.h"
#include "AiPaddle.h"
#include "Board.h"

// Constructor
AiPaddle::AiPaddle(bool is_left_paddel) : Paddle(is_left_paddel)
{
}

void AiPaddle::TrackBall(std::shared_ptr<Ball> ball)
{
  ball_ = ball;
}

/** Initialize the object.
 * Once OpenGL ready the initialize function of each object is called.
 * In this function object should initialize their OpenGL related data
 * and prepare to render.
 *
 * @return True if initialize successful and ready to update/render.
 */
/*bool AiPaddle::Initialize()
{
  return true;
}*/

/** Update the object.
 * @param fTime    Time elapsed between two updates.
 */
void AiPaddle::Update(float fTime)
{
  static float  fTotalTime = 0.0f;
  Vector2D    vBallPos;
  float      fBallDist,
          fRandom;

  // Update total time elapsed.
  fTotalTime += fTime;

  // Current ball's position.
  vBallPos = ball_->GetPosition();

  // Distance of the ball.
  if (m_bLeftPaddle)
    fBallDist = Board::GetLeft() - vBallPos.x;
  else
    fBallDist = vBallPos.x - Board::GetRight();

  // Approximation of ball's position.
  fRandom = 0.5f*Paddle::GetHeight()*cos(fTotalTime*6.0f);

  // Try to get at same height.
  if (fBallDist > 0.75f*Board::GetWidth())
    Stop(); // Don't move when ball is too far away.
  else if (m_fY < vBallPos.y-0.2f*Paddle::GetHeight()+fRandom)
    MoveUp();
  else if (m_fY > vBallPos.y+0.2f*Paddle::GetHeight()+fRandom)
    MoveDown();
  else
    Stop();

  // Update the normal paddle.
  Paddle::Update(fTime);
}

/** Process event.
 * The object receive an event to process.
 * If he has processed this event and it should not be processed by
 * any other object, then it return true.
 *
 * @param nEvent  Type of event (mouse click, keyboard, ...).
 * @param wParam  A value depending of the event type.
 * @param lParam  A value depending of the event type.
 * @return True if the message has been processed.
 */
bool AiPaddle::ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam)
{
  // Don't process events, so the player can NOT control the paddle.
  return false;
}

