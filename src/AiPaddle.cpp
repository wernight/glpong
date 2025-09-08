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

#include "AiPaddle.h"

#include <glm/glm.hpp>

#include "Board.h"

// Constructor
AiPaddle::AiPaddle(bool is_left_paddel) : Paddle(is_left_paddel) {}

void AiPaddle::TrackBall(std::shared_ptr<Ball> ball) { ball_ = ball; }

void AiPaddle::Update(float dt) {
  static float total_time = 0.0f;
  glm::vec2 ball_pos;
  float ball_distance;
  float rnd;

  // Update total time elapsed.
  total_time += dt;

  // Current ball's position.
  ball_pos = ball_->GetPosition();

  // Distance of the ball.
  if (left_paddle_)
    ball_distance = Board::GetLeft() - ball_pos.x;
  else
    ball_distance = ball_pos.x - Board::GetRight();

  // Approximation of ball's position.
  rnd = 0.5f * Paddle::GetHeight() * cos(total_time * 6.0f);

  // Try to get at same height.
  if (ball_distance > 0.75f * Board::GetWidth())
    Stop();  // Don't move when ball is too far away.
  else if (y_ < ball_pos.y - 0.2f * Paddle::GetHeight() + rnd)
    MoveUp();
  else if (y_ > ball_pos.y + 0.2f * Paddle::GetHeight() + rnd)
    MoveDown();
  else
    Stop();

  // Update the normal paddle.
  Paddle::Update(dt);
}

bool AiPaddle::ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam) {
  // Don't process events, so the player can NOT control the paddle.
  return false;
}
