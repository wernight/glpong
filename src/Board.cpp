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

#include "Board.h"

#include <vector>

#include "StdAfx.h"

constexpr float kBorderBevel = 4.0f;
constexpr float kBorderWidth = 4.0f;

constexpr float kIlluminate_Duration = 0.5f;

constexpr float kDigitHeight = 20.0f;
constexpr float kDigitWidth = 12.0f;
constexpr float kDigitBorder = 2.0f;
constexpr float kDigitInnerSpacing = 0.6f;
constexpr float kDigitSpacing = 3.0f;

namespace {
struct Vertex {
  GLfloat position[3];
  GLfloat normal[3];
};
}  // namespace

Board::Board()
    : left_score_(0),
      right_score_(0),
      illuminate_left_border_(0.0f),
      illuminate_right_border_(0.0f),
      is_game_over_(false) {
  // --- Board geometry
  std::vector<Vertex> vertices;
  // Ground
  vertices.insert(vertices.end(), {
                                      {{GetLeft(), GetTop(), 0}, {0.0f, 0.0f, -1.0f}},
                                      {{GetLeft(), GetBottom(), 0}, {0.0f, 0.0f, -1.0f}},
                                      {{GetRight(), GetBottom(), 0}, {0.0f, 0.0f, -1.0f}},
                                      {{GetRight(), GetTop(), 0}, {0.0f, 0.0f, -1.0f}},
                                  });

  // Border top
  vertices.insert(
      vertices.end(),
      {
          {{GetLeft() + kBorderWidth, GetTop() + kBorderWidth, -kBorderBevel}, {0.0f, 0.0f, -1.0f}},
          {{GetLeft(), GetTop(), -kBorderBevel}, {0.0f, 0.0f, -1.0f}},
          {{GetRight(), GetTop(), -kBorderBevel}, {0.0f, 0.0f, -1.0f}},
          {{GetRight() - kBorderWidth, GetTop() + kBorderWidth, -kBorderBevel},
           {0.0f, 0.0f, -1.0f}},

          {{GetLeft(), GetTop(), -kBorderBevel}, {0.0f, -1.0f, 0.0f}},
          {{GetLeft(), GetTop(), 0}, {0.0f, -1.0f, 0.0f}},
          {{GetRight(), GetTop(), 0}, {0.0f, -1.0f, 0.0f}},
          {{GetRight(), GetTop(), -kBorderBevel}, {0.0f, -1.0f, 0.0f}},
      });

  // Border left
  vertices.insert(
      vertices.end(),
      {
          {{GetLeft() + kBorderWidth, GetTop() + kBorderWidth, -kBorderBevel}, {0.0f, 0.0f, -1.0f}},
          {{GetLeft() + kBorderWidth, GetBottom() - kBorderWidth, -kBorderBevel},
           {0.0f, 0.0f, -1.0f}},
          {{GetLeft(), GetBottom(), -kBorderBevel}, {0.0f, 0.0f, -1.0f}},
          {{GetLeft(), GetTop(), -kBorderBevel}, {0.0f, 0.0f, -1.0f}},

          {{GetLeft(), GetTop(), -kBorderBevel}, {-1.0f, 0.0f, 0.0f}},
          {{GetLeft(), GetBottom(), -kBorderBevel}, {-1.0f, 0.0f, 0.0f}},
          {{GetLeft(), GetBottom(), 0}, {-1.0f, 0.0f, 0.0f}},
          {{GetLeft(), GetTop(), 0}, {-1.0f, 0.0f, 0.0f}},
      });

  // Border right
  vertices.insert(vertices.end(),
                  {
                      {{GetRight(), GetTop(), -kBorderBevel}, {0.0f, 0.0f, -1.0f}},
                      {{GetRight(), GetBottom(), -kBorderBevel}, {0.0f, 0.0f, -1.0f}},
                      {{GetRight() - kBorderWidth, GetBottom() - kBorderWidth, -kBorderBevel},
                       {0.0f, 0.0f, -1.0f}},
                      {{GetRight() - kBorderWidth, GetTop() + kBorderWidth, -kBorderBevel},
                       {0.0f, 0.0f, -1.0f}},

                      {{GetRight(), GetTop(), 0}, {1.0f, 0.0f, 0.0f}},
                      {{GetRight(), GetBottom(), 0}, {1.0f, 0.0f, 0.0f}},
                      {{GetRight(), GetBottom(), -kBorderBevel}, {1.0f, 0.0f, 0.0f}},
                      {{GetRight(), GetTop(), -kBorderBevel}, {1.0f, 0.0f, 0.0f}},
                  });

  // Border bottom
  vertices.insert(
      vertices.end(),
      {
          {{GetLeft(), GetBottom(), -kBorderBevel}, {0.0f, 0.0f, -1.0f}},
          {{GetLeft() + kBorderWidth, GetBottom() - kBorderWidth, -kBorderBevel},
           {0.0f, 0.0f, -1.0f}},
          {{GetRight() - kBorderWidth, GetBottom() - kBorderWidth, -kBorderBevel},
           {0.0f, 0.0f, -1.0f}},
          {{GetRight(), GetBottom(), -kBorderBevel}, {0.0f, 0.0f, -1.0f}},

          {{GetLeft() + kBorderWidth, GetBottom() - kBorderWidth, -kBorderBevel},
           {0.0f, -1.0f, 0.0f}},
          {{GetLeft() + kBorderWidth, GetBottom() - kBorderWidth, 0}, {0.0f, -1.0f, 0.0f}},
          {{GetRight() - kBorderWidth, GetBottom() - kBorderWidth, 0}, {0.0f, -1.0f, 0.0f}},
          {{GetRight() - kBorderWidth, GetBottom() - kBorderWidth, -kBorderBevel},
           {0.0f, -1.0f, 0.0f}},
      });

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, position));
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, normal));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // --- Digits geometry
  for (int i = 0; i < kDigits; ++i) {
    std::vector<GLfloat> digit_vertices;
    bool a, b, c, d, e, f, g;
    switch (i) {
      case 0:
        a = true;
        b = true;
        c = true;
        d = true;
        e = true;
        f = true;
        g = false;
        break;
      case 1:
        a = false;
        b = false;
        c = false;
        d = true;
        e = true;
        f = false;
        g = false;
        break;
      case 2:
        a = false;
        b = true;
        c = true;
        d = false;
        e = true;
        f = true;
        g = true;
        break;
      case 3:
        a = false;
        b = false;
        c = true;
        d = true;
        e = true;
        f = true;
        g = true;
        break;
      case 4:
        a = true;
        b = false;
        c = false;
        d = true;
        e = true;
        f = false;
        g = true;
        break;
      case 5:
        a = true;
        b = false;
        c = true;
        d = true;
        e = false;
        f = true;
        g = true;
        break;
      case 6:
        a = true;
        b = true;
        c = true;
        d = true;
        e = false;
        f = true;
        g = true;
        break;
      case 7:
        a = false;
        b = false;
        c = false;
        d = true;
        e = true;
        f = true;
        g = false;
        break;
      case 8:
        a = true;
        b = true;
        c = true;
        d = true;
        e = true;
        f = true;
        g = true;
        break;
      case 9:
        a = true;
        b = false;
        c = true;
        d = true;
        e = true;
        f = true;
        g = true;
        break;
    }

    if (a) {
      digit_vertices.insert(
          digit_vertices.end(),
          {0.0f, kDigitHeight - kDigitInnerSpacing, 0.0f, kDigitHeight / 2.0f + kDigitInnerSpacing,
           -kDigitBorder, kDigitHeight / 2.0f + kDigitBorder / 2.0f + kDigitInnerSpacing,
           -kDigitBorder, kDigitHeight - kDigitBorder - kDigitInnerSpacing});
    }
    if (b) {
      digit_vertices.insert(
          digit_vertices.end(),
          {0.0f, kDigitHeight / 2.0f - kDigitInnerSpacing, 0.0f, kDigitInnerSpacing, -kDigitBorder,
           kDigitBorder + kDigitInnerSpacing, -kDigitBorder,
           kDigitHeight / 2.0f - kDigitBorder / 2.0f - kDigitInnerSpacing});
    }
    if (c) {
      digit_vertices.insert(digit_vertices.end(),
                            {-kDigitInnerSpacing, 0.0f, -kDigitWidth + kDigitInnerSpacing, 0.0f,
                             -kDigitWidth + kDigitInnerSpacing + kDigitBorder, kDigitBorder,
                             -kDigitInnerSpacing - kDigitBorder, kDigitBorder});
    }
    if (d) {
      digit_vertices.insert(
          digit_vertices.end(),
          {-kDigitWidth, kDigitHeight / 2.0f - kDigitInnerSpacing, -kDigitWidth + kDigitBorder,
           kDigitHeight / 2.0f - kDigitBorder / 2.0f - kDigitInnerSpacing,
           -kDigitWidth + kDigitBorder, kDigitBorder + kDigitInnerSpacing, -kDigitWidth,
           kDigitInnerSpacing});
    }
    if (e) {
      digit_vertices.insert(
          digit_vertices.end(),
          {-kDigitWidth, kDigitHeight - kDigitInnerSpacing, -kDigitWidth + kDigitBorder,
           kDigitHeight - kDigitBorder - kDigitInnerSpacing, -kDigitWidth + kDigitBorder,
           kDigitHeight / 2.0f + kDigitBorder / 2.0f + kDigitInnerSpacing, -kDigitWidth,
           kDigitHeight / 2.0f + kDigitInnerSpacing});
    }
    if (f) {
      digit_vertices.insert(
          digit_vertices.end(),
          {-kDigitInnerSpacing, kDigitHeight, -kDigitInnerSpacing - kDigitBorder,
           kDigitHeight - kDigitBorder, -kDigitWidth + kDigitInnerSpacing + kDigitBorder,
           kDigitHeight - kDigitBorder, -kDigitWidth + kDigitInnerSpacing, kDigitHeight});
    }
    if (g) {
      digit_vertices.insert(digit_vertices.end(),
                            {-kDigitInnerSpacing, kDigitHeight / 2.0f,
                             -kDigitWidth + kDigitInnerSpacing, kDigitHeight / 2.0f,
                             -kDigitWidth + kDigitBorder, kDigitHeight / 2.0f + kDigitBorder / 2.0f,
                             -kDigitBorder, kDigitHeight / 2.0f + kDigitBorder / 2.0f});
      digit_vertices.insert(digit_vertices.end(),
                            {-kDigitInnerSpacing, kDigitHeight / 2.0f, -kDigitBorder,
                             kDigitHeight / 2.0f - kDigitBorder / 2.0f, -kDigitWidth + kDigitBorder,
                             kDigitHeight / 2.0f - kDigitBorder / 2.0f,
                             -kDigitWidth + kDigitInnerSpacing, kDigitHeight / 2.0f});
    }
    digit_vertex_counts_[i] = digit_vertices.size() / 2;

    glGenVertexArrays(1, &digit_vaos_[i]);
    glGenBuffers(1, &digit_vbos_[i]);
    glBindVertexArray(digit_vaos_[i]);
    glBindBuffer(GL_ARRAY_BUFFER, digit_vbos_[i]);
    glBufferData(GL_ARRAY_BUFFER, digit_vertices.size() * sizeof(GLfloat), digit_vertices.data(),
                 GL_STATIC_DRAW);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}

Board::~Board() {
  if (vao_) glDeleteVertexArrays(1, &vao_);
  if (vbo_) glDeleteBuffers(1, &vbo_);
  for (int i = 0; i < kDigits; ++i) {
    if (digit_vaos_[i]) glDeleteVertexArrays(1, &digit_vaos_[i]);
    if (digit_vbos_[i]) glDeleteBuffers(1, &digit_vbos_[i]);
  }
}

void Board::Reset() {
  left_score_ = right_score_ = 0;
  is_game_over_ = false;
}

void Board::Update(float fTime) {
  // Illumination.
  if (illuminate_left_border_ > 0.0f)
    illuminate_left_border_ -= fTime;
  else
    illuminate_left_border_ = 0.0f;

  if (illuminate_right_border_ > 0.0f)
    illuminate_right_border_ -= fTime;
  else
    illuminate_right_border_ = 0.0f;
}

void Board::Render() const {
  glBindVertexArray(vao_);

  // Ground
  glColor3f(0.0f, 0.15f, 0.0f);
  glDrawArrays(GL_QUADS, 0, 4);

  glColor3f(0.0f, 0.4f, 0.0f);
  // Border top
  glDrawArrays(GL_QUADS, 4, 8);

  // Border left
  glColor3f(0.0f + illuminate_left_border_, 0.4f + illuminate_left_border_,
            0.0f + illuminate_left_border_);
  glDrawArrays(GL_QUADS, 12, 8);

  // Border right
  glColor3f(0.0f + illuminate_right_border_, 0.4f + illuminate_right_border_,
            0.0f + illuminate_right_border_);
  glDrawArrays(GL_QUADS, 20, 8);

  // Border bottom
  glColor3f(0.0f, 0.4f, 0.0f);
  glDrawArrays(GL_QUADS, 28, 8);

  glBindVertexArray(0);

  // Draw score
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  glPushMatrix();
  glTranslatef(30.0f + kDigitWidth, GetTop() + 20.0f, 0.0f);
  DrawDigitNumber(left_score_);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-30.0f, GetTop() + 20.0f, 0.0f);
  DrawDigitNumber(right_score_);
  glPopMatrix();

  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
}

bool Board::ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam) {
  return false;
}

void Board::Score(bool is_left_player) {
  int *score;

  // Left or right player?
  if (is_left_player) {
    score = &left_score_;
    illuminate_left_border_ = kIlluminate_Duration;
  } else {
    score = &right_score_;
    illuminate_right_border_ = kIlluminate_Duration;
  }

  // Update player's score.
  if (*score < 30)
    *score += 15;
  else
    *score += 10;

  // The player won?
  if ((left_score_ > 40 || right_score_ > 40) && abs(left_score_ - right_score_) > 10) {
    is_game_over_ = true;
  }
}

void Board::DrawDigitNumber(int number) const {
  /*                                                   f
   __        __   __        __   __   __   __   __   a   e  _
  |  |    |  __|  __| |__| |__  |__     | |__| |__|    g   |_|
  |__|    | |__   __|    |  __| |__|    | |__|  __|  b   d | |
                                                       c
  */
  if (number == 0) {
    glBindVertexArray(digit_vaos_[0]);
    glDrawArrays(GL_QUADS, 0, digit_vertex_counts_[0]);
    glBindVertexArray(0);
    return;
  }

  do {
    int digit = number % 10;
    glBindVertexArray(digit_vaos_[digit]);
    glDrawArrays(GL_QUADS, 0, digit_vertex_counts_[digit]);
    glBindVertexArray(0);
    glTranslatef(kDigitWidth + kDigitSpacing, 0.0f, 0.0f);
  } while ((number /= 10) != 0);
}