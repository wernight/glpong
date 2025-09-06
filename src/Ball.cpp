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

#include "Ball.h"

#include <vector>

#include "StdAfx.h"

constexpr float kBallSpeed = 110.0f;
constexpr float kBallSpeedIncrease = 5.0f;
constexpr float kBallRadius = 2.0f;
constexpr float kBallMaxAngle = M_PI / 3.0f;  // y = a*x
constexpr float kBallMinAngle = M_PI / 7.0f;  // y = a*x
constexpr float kPartSize = 1.7f;

namespace {
struct Vertex {
  GLfloat position[3];
  GLfloat normal[3];
};

struct ParticleVertex {
  GLfloat position[3];
  GLfloat texcoord[2];
};

void generateSphere(std::vector<Vertex> &vertices, float radius, int rings, int sectors) {
  float const R = 1. / (float)(rings - 1);
  float const S = 1. / (float)(sectors - 1);
  int r, s;

  vertices.resize(rings * sectors * 6);
  auto it = vertices.begin();
  for (r = 0; r < rings - 1; r++)
    for (s = 0; s < sectors - 1; s++) {
      float const y0 = sin(-M_PI_2 + M_PI * r * R);
      float const x0 = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
      float const z0 = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

      float const y1 = sin(-M_PI_2 + M_PI * (r + 1) * R);
      float const x1 = cos(2 * M_PI * s * S) * sin(M_PI * (r + 1) * R);
      float const z1 = sin(2 * M_PI * s * S) * sin(M_PI * (r + 1) * R);

      float const y2 = sin(-M_PI_2 + M_PI * r * R);
      float const x2 = cos(2 * M_PI * (s + 1) * S) * sin(M_PI * r * R);
      float const z2 = sin(2 * M_PI * (s + 1) * S) * sin(M_PI * r * R);

      float const y3 = sin(-M_PI_2 + M_PI * (r + 1) * R);
      float const x3 = cos(2 * M_PI * (s + 1) * S) * sin(M_PI * (r + 1) * R);
      float const z3 = sin(2 * M_PI * (s + 1) * S) * sin(M_PI * (r + 1) * R);

      it->position[0] = x0 * radius;
      it->position[1] = y0 * radius;
      it->position[2] = z0 * radius;
      it->normal[0] = x0;
      it->normal[1] = y0;
      it->normal[2] = z0;
      ++it;
      it->position[0] = x1 * radius;
      it->position[1] = y1 * radius;
      it->position[2] = z1 * radius;
      it->normal[0] = x1;
      it->normal[1] = y1;
      it->normal[2] = z1;
      ++it;
      it->position[0] = x2 * radius;
      it->position[1] = y2 * radius;
      it->position[2] = z2 * radius;
      it->normal[0] = x2;
      it->normal[1] = y2;
      it->normal[2] = z2;
      ++it;
      it->position[0] = x1 * radius;
      it->position[1] = y1 * radius;
      it->position[2] = z1 * radius;
      it->normal[0] = x1;
      it->normal[1] = y1;
      it->normal[2] = z1;
      ++it;
      it->position[0] = x3 * radius;
      it->position[1] = y3 * radius;
      it->position[2] = z3 * radius;
      it->normal[0] = x3;
      it->normal[1] = y3;
      it->normal[2] = z3;
      ++it;
      it->position[0] = x2 * radius;
      it->position[1] = y2 * radius;
      it->position[2] = z2 * radius;
      it->normal[0] = x2;
      it->normal[1] = y2;
      it->normal[2] = z2;
      ++it;
    }
}
}  // namespace

Ball::Ball(std::shared_ptr<Board> board, std::shared_ptr<Paddle> left_paddle,
           std::shared_ptr<Paddle> right_paddle, GLuint texture)
    : board_(board), left_paddle_(left_paddle), right_paddle_(right_paddle), texture_(texture) {
  // Create a new ball.
  ball_position_.y = 0.0f;
  NewBall(rand_.RandomInt() % 2 == 0);

  // Init particles.
  for (auto &part : particles_) {
    part.life = 1.0f;
    part.fade = (float)rand_.RandomRange(3.0f, 28.0f);  // Random Fade Value
    part.x = ball_speed_.x;
    part.y = ball_speed_.y;
    part.z = -kBallRadius;
  }

  // Initiliaz random number generator.
  rand_.Randomize();

  // Ball sphere
  std::vector<Vertex> vertices;
  generateSphere(vertices, kBallRadius, 7, 5);
  sphere_vertex_count_ = vertices.size();

  glGenVertexArrays(1, &sphere_vao_);
  glGenBuffers(1, &sphere_vbo_);
  glBindVertexArray(sphere_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, position));
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, normal));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Particles
  glGenVertexArrays(1, &particles_vao_);
  glGenBuffers(1, &particles_vbo_);
  glBindVertexArray(particles_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, particles_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(particles_) * 6 * sizeof(ParticleVertex), nullptr,
               GL_DYNAMIC_DRAW);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(ParticleVertex), (void *)offsetof(ParticleVertex, position));
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(ParticleVertex),
                    (void *)offsetof(ParticleVertex, texcoord));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

Ball::~Ball() {
  if (sphere_vao_) glDeleteVertexArrays(1, &sphere_vao_);
  if (sphere_vbo_) glDeleteBuffers(1, &sphere_vbo_);
  if (particles_vao_) glDeleteVertexArrays(1, &particles_vao_);
  if (particles_vbo_) glDeleteBuffers(1, &particles_vbo_);
}

/** Update the object.
 * @param fTime    Time elapsed between two updates.
 */
void Ball::Update(float fTime) {
  Vector2D new_ball_pos(ball_position_ + ball_speed_ * fTime);
  double dAngle, dSpeed;
  int i;

  // Bounce top/bottom
  if (new_ball_pos.y + kBallRadius > Board::GetTop()) {
    ball_speed_.y = -ball_speed_.y;
    new_ball_pos.y = 2.0f * (Board::GetTop() - kBallRadius) - new_ball_pos.y;
  } else if (new_ball_pos.y - kBallRadius < Board::GetBottom()) {
    ball_speed_.y = -ball_speed_.y;
    new_ball_pos.y = 2.0f * (Board::GetBottom() + kBallRadius) - new_ball_pos.y;
  }

  // Left paddle collision detection.
  if (new_ball_pos.x + kBallRadius > Board::GetLeft() - Paddle::GetWidth()) {
    // y = a*x + b
    float a = ball_speed_.y / ball_speed_.x, b = ball_position_.y - a * ball_position_.x, y;

    // Bounce on paddle?
    if (ball_position_.x + kBallRadius <= Board::GetLeft() - Paddle::GetWidth() &&
        (y = a * (Board::GetLeft() - Paddle::GetWidth() - kBallRadius) + b) - kBallRadius <=
            left_paddle_->GetPosition() + Paddle::GetHeight() * 0.5f &&
        y + kBallRadius >= left_paddle_->GetPosition() - Paddle::GetHeight() * 0.5f) {
      // Illuminate.
      left_paddle_->Illuminate();
      // Bounce.
      new_ball_pos.x =
          2.0f * (Board::GetLeft() - Paddle::GetWidth() - kBallRadius) - new_ball_pos.x;
      // Bouce angle.
      dAngle = (left_paddle_->GetPosition() - new_ball_pos.y) * M_PI / 4.0f /
                   (Paddle::GetHeight() / 2.0f) +
               M_PI;

      // Increase speed
      dSpeed = ball_speed_.Norm() + kBallSpeedIncrease;
      ball_speed_.x = float(cos(dAngle) * dSpeed);
      ball_speed_.y = float(sin(dAngle) * dSpeed);
    }
    /*    // Bounce on corners?
        else if (HitPoint(ball_position_, new_ball_pos, ball_speed_,
       Vector2D(Board::GetLeft()-Paddle::GetWidth(),
       left_paddle_->GetPosition()+Paddle::GetHeight()*0.5f)) || HitPoint(ball_position_,
       new_ball_pos, ball_speed_, Vector2D(Board::GetLeft()-Paddle::GetWidth(),
       left_paddle_->GetPosition()-Paddle::GetHeight()*0.5f))) { }*/
    // Score?
    else if (new_ball_pos.x + kBallRadius > Board::GetLeft()) {
      board_->Score(true);
      NewBall(true);
      new_ball_pos = ball_position_;
    }
  }
  // Right paddle collision detection.
  else if (new_ball_pos.x - kBallRadius < Board::GetRight() + Paddle::GetWidth()) {
    // y = a*x + b
    float a = ball_speed_.y / ball_speed_.x, b = ball_position_.y - a * ball_position_.x, y;

    // Bounce on paddle?
    if (ball_position_.x - kBallRadius >= Board::GetRight() + Paddle::GetWidth() &&
        (y = a * (Board::GetRight() + Paddle::GetWidth() + kBallRadius) + b) - kBallRadius <=
            right_paddle_->GetPosition() + Paddle::GetHeight() * 0.5f &&
        y + kBallRadius >= right_paddle_->GetPosition() - Paddle::GetHeight() * 0.5f) {
      // Illuminate.
      right_paddle_->Illuminate();
      // Bounce.
      new_ball_pos.x =
          2.0f * (Board::GetRight() + Paddle::GetWidth() + kBallRadius) - new_ball_pos.x;
      // Bouce angle.
      dAngle = (new_ball_pos.y - right_paddle_->GetPosition()) * M_PI / 4.0f /
               (Paddle::GetHeight() / 2.0f);

      // Increase speed
      dSpeed = ball_speed_.Norm() + kBallSpeedIncrease;
      ball_speed_.x = float(cos(dAngle) * dSpeed);
      ball_speed_.y = float(sin(dAngle) * dSpeed);
    }
    /*    // Bounce on corners?
        else if (HitPoint(ball_position_, new_ball_pos, ball_speed_,
       Vector2D(Board::GetRight()+Paddle::GetWidth(),
       right_paddle_->GetPosition()+Paddle::GetHeight()*0.5f)) || HitPoint(ball_position_,
       new_ball_pos, ball_speed_, Vector2D(Board::GetRight()+Paddle::GetWidth(),
       right_paddle_->GetPosition()-Paddle::GetHeight()*0.5f))) { }*/
    // Score?
    else if (new_ball_pos.x - kBallRadius < Board::GetRight()) {
      board_->Score(false);
      NewBall(false);
      new_ball_pos = ball_position_;
    }
  }

  // Ball's position.
  ball_position_ = new_ball_pos;

  // Particles.
  for (auto &part : particles_) {
    // Reduce Particles Life By 'Fade'
    part.life -= part.fade * fTime;

    // Regenerate if Particle is Burned Out
    if (part.life >= 0.0f) continue;

    part.life = 1.0f;
    part.fade = (float)rand_.RandomRange(3.0f, 28.0f);  // Random Fade Value
    part.x = ball_position_.x + (float)rand_.RandomRange(-kBallRadius * 0.5f, kBallRadius * 0.5f);
    part.y = ball_position_.y + (float)rand_.RandomRange(-kBallRadius * 0.5f, kBallRadius * 0.5f);
    part.z = -kBallRadius + (float)rand_.RandomRange(-kBallRadius * 0.5f, kBallRadius * 0.5f);
  }
}

void Ball::Render() const {
  // Ball
  glPushMatrix();
  glTranslatef(ball_position_.x, ball_position_.y, -kBallRadius);
  glColor3f(0.0f, 1.0f, 0.0f);
  glBindVertexArray(sphere_vao_);
  glDrawArrays(GL_TRIANGLES, 0, sphere_vertex_count_);
  glBindVertexArray(0);
  glPopMatrix();

  // Particules
  if (texture_ != 0) glBindTexture(GL_TEXTURE_2D, texture_);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  std::vector<ParticleVertex> vertices;
  vertices.reserve(sizeof(particles_) / sizeof(particles_[0]) * 6);

  float model_view[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, model_view);
  Vector3D right(model_view[0], model_view[4], model_view[8]);
  Vector3D up(model_view[1], model_view[5], model_view[9]);

  for (auto &part : particles_) {
    float ps = part.life * kPartSize;
    Vector3D center(part.x, part.y, part.z);

    Vector3D tl = center - (right + up) * ps;
    Vector3D tr = center + (right - up) * ps;
    Vector3D br = center + (right + up) * ps;
    Vector3D bl = center - (right - up) * ps;

    vertices.push_back({{tl.x, tl.y, tl.z}, {0, 1}});
    vertices.push_back({{br.x, br.y, br.z}, {1, 0}});
    vertices.push_back({{tr.x, tr.y, tr.z}, {1, 1}});
    vertices.push_back({{tl.x, tl.y, tl.z}, {0, 1}});
    vertices.push_back({{bl.x, bl.y, bl.z}, {0, 0}});
    vertices.push_back({{br.x, br.y, br.z}, {1, 0}});
  }

  glColor3f(0.0f, 1.0f, 0.0f);
  glBindVertexArray(particles_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, particles_vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(ParticleVertex), vertices.data());
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

bool Ball::ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam) { return false; }

void Ball::NewBall(bool bGoToLeft) {
  // Selects a random angle.
  float angle;
  do angle = (float)rand_.RandomRange(-kBallMaxAngle, +kBallMaxAngle);
  while (fabs(angle) < kBallMinAngle);

  if (bGoToLeft) {
    angle += float(M_PI);
    ball_position_.x = Board::GetLeft() - Paddle::GetWidth();
  } else
    ball_position_.x = Board::GetRight() + Paddle::GetWidth();
  ball_speed_.x = (float)cos(angle) * kBallSpeed;
  ball_speed_.y = (float)sin(angle) * kBallSpeed;
}

inline bool Ball::HitPoint(Vector2D &old_pos, Vector2D &new_pos, Vector2D &speed, Vector2D &a_pos) {
  // Position where distance from line y=a*x+b to the point A is the shortest.
  float a = speed.y / speed.x;
  float b = old_pos.y - a * old_pos.x;
  float t0 = -a * a * (a_pos.x * a_pos.x - kBallRadius * kBallRadius) -
             2.0f * a * (b - a_pos.y) * a_pos.x - b * b + 2.0f * b * a_pos.y - a_pos.y * a_pos.y +
             kBallRadius * kBallRadius;
  // Check if hit.
  if (t0 < 0) return false;
  // Find hit position.
  float t1 = sqrt(t0), t2 = -a * (b - a_pos.y) + a_pos.x, t3 = a * a + 1, x1 = (+t1 + t2) / t3,
        x2 = (-t1 + t2) / t3, x;
  // Hit position is one of the two x coordiantes.
  if (fabs(old_pos.x - x1) < fabs(old_pos.x - x2))
    x = x1;
  else
    x = x2;
  // Is hit position is between old_pos and new_pos?
  if (!(old_pos.x <= x && x <= new_pos.x || new_pos.x <= x && x <= old_pos.x)) return false;

  // The shortest vector from A to the line defined by y=a*x+b.
  Vector2D vect(x - a_pos.x, a * x + b - a_pos.y);

  // Update speed vector.
  speed = vect * ((speed.Norm() + kBallSpeedIncrease) / vect.Norm());
  // Update position.
  //! TODO
  new_pos = old_pos;
  // Temporary position version that gives bad results on slow machines.
  /*
  R is the new direction vector
  I is the old direction vector before the collision
  N is the Normal at the collision point

  The new vector R is calculated as follows:

  R= 2*(-I dot N)*N + I

  The restriction is that the I and N vectors have to be unit vectors. The velocity vector as used
  in our examples represents speed and direction. Therefore it can not be plugged into the equation
  in the place of I, without any transformation. The speed has to be extracted. The speed for such a
  velocity vector is extracted finding the magnitude of the vector. Once the magnitude is found, the
  vector can be transformed to a unit vector and plugged into the equation giving the reflection
  vector R. R shows us now the direction, of the reflected ray, but in order to be used as a
  velocity vector it must also incorporate the speed. Therefore it gets, multiplied with the
  magnitude of the original ray, thus resulting in the correct velocity vector.
  */
  return true;
}