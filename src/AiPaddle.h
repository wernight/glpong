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
#include "Paddle.h"

class AiPaddle : public Paddle {
 public:
  // Constructor
  AiPaddle(bool is_left_paddel);

  void TrackBall(std::shared_ptr<Ball> ball);

  // Overrides
  // Update the object.
  void Update(float dt) override;

  // Process event.
  bool ProcessEvent(const SDL_Event& event) override;

  // Implementation
 private:
  std::shared_ptr<Ball> ball_;
};
