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

#include <array>
#include <glm/mat4x4.hpp>
#include <memory>

#include "IObject.h"

class Shader;

class Board : public IObject {
 public:
  // Constructor
  Board();
  virtual ~Board();

  void Reset();

  // Update the object.
  void Update(float dt) override;

  // Render the object.
  void Render(const glm::mat4& view, const glm::mat4& model,
              const glm::mat4& projection) const override;

  // Process event.
  bool ProcessEvent(const SDL_Event& event) override;

  static const float GetTop() { return 48.0f; }
  static const float GetBottom() { return -48.0f; }
  static const float GetLeft() { return 64.0f; }
  static const float GetRight() { return -64.0f; }

  static const float GetWidth() { return GetLeft() - GetRight(); }
  static const float GetHeight() { return GetTop() - GetBottom(); }

  bool IsGameOver() const { return is_game_over_; }

  // Add points to a player's score.
  void Score(bool left_player);

 private:
  void DrawDigitNumber(int number, glm::mat4 modelview) const;

  int left_score_;
  int right_score_;
  float illuminate_left_border_;   // Illuminate the left border.
  float illuminate_right_border_;  // Illuminate the right border.
  bool is_game_over_;

  GLuint vao_ = 0;
  GLuint vbo_ = 0;

  static constexpr int kDigits = 10;
  std::array<GLuint, kDigits> digit_vaos_ = {0};
  std::array<GLuint, kDigits> digit_vbos_ = {0};
  std::array<int, kDigits> digit_vertex_counts_ = {0};

  std::unique_ptr<Shader> board_shader_;
  std::unique_ptr<Shader> digit_shader_;
};
