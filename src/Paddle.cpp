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

#include "Paddle.h"

#include <vector>

#include "Board.h"

constexpr float kPaddleBevel = 4.0f;
constexpr float kPaddleSpeed = 150.0f;
constexpr float kPaddleIlluminate = 0.5f;
constexpr float kPaddleIlluminateFade = 0.3f;

namespace {

struct Vertex {
  GLfloat position[3];
  GLfloat normal[3];
};

}  // namespace

Paddle::Paddle(bool is_left_paddle)
    : speed_(0.0f), y_(0.0f), illuminate_(0.0f), left_paddle_(is_left_paddle) {
  std::vector<Vertex> vertices;
  if (left_paddle_) {
    vertices.insert(
        vertices.end(),
        {
            // Front face
            {{Board::GetLeft(), GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, 0.0f, -1.0f}},
            {{Board::GetLeft(), -GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, 0.0f, -1.0f}},
            {{Board::GetLeft() - GetWidth(), -GetHeight() / 2.0f, -kPaddleBevel},
             {0.0f, 0.0f, -1.0f}},
            {{Board::GetLeft() - GetWidth(), GetHeight() / 2.0f, -kPaddleBevel},
             {0.0f, 0.0f, -1.0f}},
            // Left face
            {{Board::GetLeft() - GetWidth(), GetHeight() / 2.0f, -kPaddleBevel},
             {-1.0f, 0.0f, 0.0f}},
            {{Board::GetLeft() - GetWidth(), -GetHeight() / 2.0f, -kPaddleBevel},
             {-1.0f, 0.0f, 0.0f}},
            {{Board::GetLeft() - GetWidth(), -GetHeight() / 2.0f, 0}, {-1.0f, 0.0f, 0.0f}},
            {{Board::GetLeft() - GetWidth(), GetHeight() / 2.0f, 0}, {-1.0f, 0.0f, 0.0f}},
            // Bottom face
            {{Board::GetLeft(), -GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, -1.0f, 0.0f}},
            {{Board::GetLeft(), -GetHeight() / 2.0f, 0}, {0.0f, -1.0f, 0.0f}},
            {{Board::GetLeft() - GetWidth(), -GetHeight() / 2.0f, 0}, {0.0f, -1.0f, 0.0f}},
            {{Board::GetLeft() - GetWidth(), -GetHeight() / 2.0f, -kPaddleBevel},
             {0.0f, -1.0f, 0.0f}},
        });
  } else {
    vertices.insert(
        vertices.end(),
        {
            // Front face
            {{Board::GetRight() + GetWidth(), GetHeight() / 2.0f, -kPaddleBevel},
             {0.0f, 0.0f, -1.0f}},
            {{Board::GetRight() + GetWidth(), -GetHeight() / 2.0f, -kPaddleBevel},
             {0.0f, 0.0f, -1.0f}},
            {{Board::GetRight(), -GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, 0.0f, -1.0f}},
            {{Board::GetRight(), GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, 0.0f, -1.0f}},
            // Right face
            {{Board::GetRight() + GetWidth(), GetHeight() / 2.0f, 0}, {1.0f, 0.0f, 0.0f}},
            {{Board::GetRight() + GetWidth(), -GetHeight() / 2.0f, 0}, {1.0f, 0.0f, 0.0f}},
            {{Board::GetRight() + GetWidth(), -GetHeight() / 2.0f, -kPaddleBevel},
             {1.0f, 0.0f, 0.0f}},
            {{Board::GetRight() + GetWidth(), GetHeight() / 2.0f, -kPaddleBevel},
             {1.0f, 0.0f, 0.0f}},
            // Bottom face
            {{Board::GetRight() + GetWidth(), -GetHeight() / 2.0f, -kPaddleBevel},
             {0.0f, -1.0f, 0.0f}},
            {{Board::GetRight() + GetWidth(), -GetHeight() / 2.0f, 0}, {0.0f, -1.0f, 0.0f}},
            {{Board::GetRight(), -GetHeight() / 2.0f, 0}, {0.0f, -1.0f, 0.0f}},
            {{Board::GetRight(), -GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, -1.0f, 0.0f}},
        });
  }
  vertex_count_ = vertices.size();

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);

  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));

  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

Paddle::~Paddle() {
  if (vao_) glDeleteVertexArrays(1, &vao_);
  if (vbo_) glDeleteBuffers(1, &vbo_);
}

void Paddle::Update(float fTime) {
  // Update paddle position.
  y_ += speed_ * fTime;
  if (y_ > Board::GetTop() - GetHeight() / 2.0f) {
    y_ = Board::GetTop() - GetHeight() / 2.0f;
    ;
    speed_ = 0.0f;
  }
  if (y_ < Board::GetBottom() + GetHeight() / 2.0f) {
    y_ = Board::GetBottom() + GetHeight() / 2.0f;
    speed_ = 0.0f;
  }

  // Fade hightlight.
  if (illuminate_ > 0.0f)
    illuminate_ -= kPaddleIlluminateFade * fTime;
  else
    illuminate_ = 0.0f;
}

void Paddle::Render() const {
  // Paddle.
  glPushMatrix();
  glTranslatef(0, y_, 0);
  glColor3f(illuminate_, 1.0f, illuminate_);

  glBindVertexArray(vao_);
  glDrawArrays(GL_QUADS, 0, vertex_count_);
  glBindVertexArray(0);

  glPopMatrix();
}

bool Paddle::ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam) {
  switch (nEvent) {
    case eventKeyDown:
      switch (wParam) {
        case SDLK_LSHIFT:
          if (left_paddle_) {
            MoveUp();
            return true;
          }
          break;
        case SDLK_LCTRL:
          if (left_paddle_) {
            MoveDown();
            return true;
          }
          break;
        case SDLK_UP:
          if (!left_paddle_) {
            MoveUp();
            return true;
          }
          break;
        case SDLK_DOWN:
          if (!left_paddle_) {
            MoveDown();
            return true;
          }
          break;
      }
      break;

    case eventKeyUp:
      switch (lParam) {
        case SDLK_LSHIFT:
        case SDLK_LCTRL:
          if (left_paddle_) {
            Stop();
            return true;
          }
          break;
        case SDLK_UP:
        case SDLK_DOWN:
          if (!left_paddle_) {
            Stop();
            return true;
          }
          break;
      }
      break;

    case eventMouseMove:
    case eventMouseClick:
    case eventChar:
      break;
  }

  return false;
}

void Paddle::MoveUp() { speed_ = kPaddleSpeed; }

void Paddle::MoveDown() { speed_ = -kPaddleSpeed; }

void Paddle::Stop() { speed_ = 0.0f; }

void Paddle::Illuminate() { illuminate_ = kPaddleIlluminate; }