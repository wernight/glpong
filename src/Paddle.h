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

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <memory>

#include "IObject.h"

class Shader;
class Ball;

class Paddle : public IObject {
 public:
  // Constructor
  Paddle(bool left_paddle);
  virtual ~Paddle();

  // Implementation of IObject.
  // Update the object.
  void Update(float fTime) override;

  // Render the object.
  void Render(const glm::mat4& view, const glm::mat4& model,
              const glm::mat4& projection) const override;

  // Process event.
  bool ProcessEvent(const SDL_Event& event) override;

  void TrackBall(std::shared_ptr<Ball> ball);

  // Attributes
  static const float GetWidth() { return 6.0f; }

  static const float GetHeight() { return 20.0f; }

  inline float GetPosition() const { return y_; }

  // Operations
  // Move paddle upward.
  void MoveUp();

  // Move paddle downward.
  void MoveDown();

  // Stop moving paddle.
  void Stop();

  // Illuminate paddle.
  void Illuminate();

  // Implementation
 protected:
  bool left_paddle_;
  float speed_;
  float y_;
  float illuminate_;
  GLuint vao_ = 0;
  GLuint vbo_ = 0;
  int vertex_count_ = 0;
  std::unique_ptr<Shader> shader_;
  std::shared_ptr<Ball> ball_;
  // We start assuming that there was no human input.
  float time_since_last_input_ = 99.0f;
  float total_time_ = 0.0f;
};
