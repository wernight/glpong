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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

constexpr float kBallSpeed = 110.0f;
constexpr float kBallSpeedIncrease = 5.0f;
constexpr float kBallRadius = 2.0f;
constexpr float kBallMaxAngle = M_PI / 3.0f;  // y = a*x
constexpr float kBallMinAngle = M_PI / 7.0f;  // y = a*x
constexpr float kPartSize = 1.7f;

namespace {
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

struct ParticleVertex {
  glm::vec3 position;
  glm::vec2 texcoord;
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

      it->position = {x0 * radius, y0 * radius, z0 * radius};
      it->normal = {x0, y0, z0};
      ++it;
      it->position = {x1 * radius, y1 * radius, z1 * radius};
      it->normal = {x1, y1, z1};
      ++it;
      it->position = {x2 * radius, y2 * radius, z2 * radius};
      it->normal = {x2, y2, z2};
      ++it;
      it->position = {x1 * radius, y1 * radius, z1 * radius};
      it->normal = {x1, y1, z1};
      ++it;
      it->position = {x3 * radius, y3 * radius, z3 * radius};
      it->normal = {x3, y3, z3};
      ++it;
      it->position = {x2 * radius, y2 * radius, z2 * radius};
      it->normal = {x2, y2, z2};
      ++it;
    }
}
}  // namespace

Ball::Ball(std::shared_ptr<Board> board, std::shared_ptr<Paddle> left_paddle,
           std::shared_ptr<Paddle> right_paddle, GLuint texture)
    : board_(board),
      left_paddle_(left_paddle),
      right_paddle_(right_paddle),
      texture_(texture),
      gen_(std::random_device()()),
      fade_dist_(3.0f, 28.0f) {
  // Create a new ball.
  ball_position_.y = 0.0f;
  bool go_left = std::uniform_int_distribution(0, 1)(gen_) == 0;
  NewBall(go_left);

  // Init particles.
  for (auto &part : particles_) {
    part.life = 1.0f;
    part.fade = fade_dist_(gen_);  // Random Fade Value
    part.pos.x = ball_speed_.x;
    part.pos.y = ball_speed_.y;
    part.pos.z = -kBallRadius;
  }

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
  glBufferData(GL_ARRAY_BUFFER, particles_.size() * 6 * sizeof(ParticleVertex), nullptr,
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

void Ball::Update(float dt) {
  glm::vec2 new_ball_pos(ball_position_ + ball_speed_ * dt);
  // Bounce top/bottom
  if (new_ball_pos.y + kBallRadius > Board::GetTop()) {
    ball_speed_.y = -ball_speed_.y;
    new_ball_pos.y = 2.0f * (Board::GetTop() - kBallRadius) - new_ball_pos.y;
  } else if (new_ball_pos.y - kBallRadius < Board::GetBottom()) {
    ball_speed_.y = -ball_speed_.y;
    new_ball_pos.y = 2.0f * (Board::GetBottom() + kBallRadius) - new_ball_pos.y;
  }

  if (new_ball_pos.x + kBallRadius > Board::GetLeft() - Paddle::GetWidth()) {
    // Left paddle collision detection.
    // y = a*x + b
    float a = ball_speed_.y / ball_speed_.x;
    float b = ball_position_.y - a * ball_position_.x;
    float y = a * (Board::GetLeft() - Paddle::GetWidth() - kBallRadius) + b;

    if (ball_position_.x + kBallRadius <= Board::GetLeft() - Paddle::GetWidth() &&
        y - kBallRadius <= left_paddle_->GetPosition() + Paddle::GetHeight() * 0.5f &&
        y + kBallRadius >= left_paddle_->GetPosition() - Paddle::GetHeight() * 0.5f) {
      // Illuminate.
      left_paddle_->Illuminate();
      // Bounce on paddle.
      new_ball_pos.x =
          2.0f * (Board::GetLeft() - Paddle::GetWidth() - kBallRadius) - new_ball_pos.x;
      // Bouce angle.
      double angle = (left_paddle_->GetPosition() - new_ball_pos.y) * M_PI / 4.0f /
                         (Paddle::GetHeight() / 2.0f) +
                     M_PI;

      // Increase speed
      double speed = glm::length(ball_speed_) + kBallSpeedIncrease;
      ball_speed_.x = float(cos(angle) * speed);
      ball_speed_.y = float(sin(angle) * speed);
    } else if (new_ball_pos.x + kBallRadius > Board::GetLeft()) {
      // Score
      board_->Score(true);
      NewBall(true);
      new_ball_pos = ball_position_;
    }
  } else if (new_ball_pos.x - kBallRadius < Board::GetRight() + Paddle::GetWidth()) {
    // Right paddle collision detection.
    // y = a*x + b
    float a = ball_speed_.y / ball_speed_.x;
    float b = ball_position_.y - a * ball_position_.x;
    float y = a * (Board::GetRight() + Paddle::GetWidth() + kBallRadius) + b;

    // Bounce on paddle?
    if (ball_position_.x - kBallRadius >= Board::GetRight() + Paddle::GetWidth() &&
        y - kBallRadius <= right_paddle_->GetPosition() + Paddle::GetHeight() * 0.5f &&
        y + kBallRadius >= right_paddle_->GetPosition() - Paddle::GetHeight() * 0.5f) {
      // Illuminate.
      right_paddle_->Illuminate();
      // Bounce.
      new_ball_pos.x =
          2.0f * (Board::GetRight() + Paddle::GetWidth() + kBallRadius) - new_ball_pos.x;
      // Bouce angle.
      double angle = (new_ball_pos.y - right_paddle_->GetPosition()) * M_PI / 4.0f /
                     (Paddle::GetHeight() / 2.0f);

      // Increase speed
      double speed = glm::length(ball_speed_) + kBallSpeedIncrease;
      ball_speed_.x = float(cos(angle) * speed);
      ball_speed_.y = float(sin(angle) * speed);
    } else if (new_ball_pos.x - kBallRadius < Board::GetRight()) {
      // Score
      board_->Score(false);
      NewBall(false);
      new_ball_pos = ball_position_;
    }
  }

  // Ball's position.
  ball_position_ = new_ball_pos;

  // Particles.
  std::uniform_real_distribution<float> pos_dist(-kBallRadius * 0.5f, kBallRadius * 0.5f);
  for (auto &part : particles_) {
    // Reduce Particles Life By 'Fade'
    part.life -= part.fade * dt;

    // Regenerate if Particle is Burned Out
    if (part.life >= 0.0f) continue;

    part.life = 1.0f;
    part.fade = fade_dist_(gen_);  // Random Fade Value
    part.pos.x = ball_position_.x + pos_dist(gen_);
    part.pos.y = ball_position_.y + pos_dist(gen_);
    part.pos.z = -kBallRadius + pos_dist(gen_);
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
  vertices.reserve(particles_.size() * 6);

  glm::mat4 model_view;
  glGetFloatv(GL_MODELVIEW_MATRIX, &model_view[0][0]);
  glm::vec3 right(model_view[0][0], model_view[1][0], model_view[2][0]);
  glm::vec3 up(model_view[0][1], model_view[1][1], model_view[2][1]);

  for (auto &part : particles_) {
    float ps = part.life * kPartSize;
    glm::vec3 center = part.pos;

    glm::vec3 tl = center - (right + up) * ps;
    glm::vec3 tr = center + (right - up) * ps;
    glm::vec3 br = center + (right + up) * ps;
    glm::vec3 bl = center - (right - up) * ps;

    vertices.push_back({tl, {0, 1}});
    vertices.push_back({br, {1, 0}});
    vertices.push_back({tr, {1, 1}});
    vertices.push_back({tl, {0, 1}});
    vertices.push_back({bl, {0, 0}});
    vertices.push_back({br, {1, 0}});
  }

  glColor3f(0.0f, 1.0f, 0.0f);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindVertexArray(particles_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, particles_vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(ParticleVertex), vertices.data());
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

bool Ball::ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam) { return false; }

void Ball::NewBall(bool go_to_left) {
  // Selects a random angle.
  std::uniform_real_distribution<float> angle_dist(-kBallMaxAngle, +kBallMaxAngle);
  float angle;
  do angle = angle_dist(gen_);
  while (fabs(angle) < kBallMinAngle);

  if (go_to_left) {
    angle += float(M_PI);
    ball_position_.x = Board::GetLeft() - Paddle::GetWidth();
  } else
    ball_position_.x = Board::GetRight() + Paddle::GetWidth();
  ball_speed_.x = (float)cos(angle) * kBallSpeed;
  ball_speed_.y = (float)sin(angle) * kBallSpeed;
}
