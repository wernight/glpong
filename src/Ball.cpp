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
 * Copyright (c) 2003-2024 Werner BEROUX
 * Mail: werner@beroux.com
 * Web : www.beroux.com
 */

#include "Ball.h"

#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

constexpr float kBallSpeed = 110.0f;
constexpr float kBallSpeedIncrease = 5.0f;
constexpr float kBallRadius = 2.0f;
constexpr float kBallMaxAngle = M_PI / 3.0f;  // y = a*x
constexpr float kBallMinAngle = M_PI / 7.0f;  // y = a*x

Ball::Ball(std::shared_ptr<Board> board, std::shared_ptr<Paddle> left_paddle,
           std::shared_ptr<Paddle> right_paddle, GLuint texture)
    : board_(board),
      left_paddle_(left_paddle),
      right_paddle_(right_paddle),
      particle_shader_(texture, particles_.size()),
      gen_(std::random_device()()),
      fade_dist_(3.0f, 28.0f) {
  // Create a new ball.
  ball_position_.y = 0.0f;
  bool go_left = std::uniform_int_distribution(0, 1)(gen_) == 0;
  NewBall(go_left);

  // Init particles.
  for (auto& part : particles_) {
    part.life = 1.0f;
    part.fade = fade_dist_(gen_);  // Random Fade Value
    part.pos.x = ball_speed_.x;
    part.pos.y = ball_speed_.y;
    part.pos.z = -kBallRadius;
  }
}

Ball::~Ball() {}

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

    bool ball_is_touching_paddle_front_edge =
        ball_position_.x + kBallRadius <= Board::GetLeft() - Paddle::GetWidth() &&
        y - kBallRadius <= left_paddle_->GetPosition() + Paddle::GetHeight() * 0.5f &&
        y + kBallRadius >= left_paddle_->GetPosition() - Paddle::GetHeight() * 0.5f;
    if (ball_is_touching_paddle_front_edge) {
      // Illuminate the pad.
      left_paddle_->Illuminate();
      // Bounce on the pad.
      new_ball_pos.x =
          2.0f * (Board::GetLeft() - Paddle::GetWidth() - kBallRadius) - new_ball_pos.x;
      double angle =
          (left_paddle_->GetPosition() - new_ball_pos.y) / Paddle::GetHeight() * M_PI / 2.0f + M_PI;

      // Increase the ball's speed.
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
    bool ball_is_touching_paddle_front_edge =
        ball_position_.x - kBallRadius >= Board::GetRight() + Paddle::GetWidth() &&
        y - kBallRadius <= right_paddle_->GetPosition() + Paddle::GetHeight() * 0.5f &&
        y + kBallRadius >= right_paddle_->GetPosition() - Paddle::GetHeight() * 0.5f;
    if (ball_is_touching_paddle_front_edge) {
      // Illuminate the pad.
      right_paddle_->Illuminate();
      // Bounce on the pad.
      new_ball_pos.x =
          2.0f * (Board::GetRight() + Paddle::GetWidth() + kBallRadius) - new_ball_pos.x;
      double angle =
          (new_ball_pos.y - right_paddle_->GetPosition()) / Paddle::GetHeight() * M_PI / 2.0f;

      // Increase the ball's speed.
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
  for (auto& part : particles_) {
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

void Ball::Render(const glm::mat4& model, const glm::mat4& view,
                  const glm::mat4& projection) const {
  auto color = glm::vec3(0.0f, 1.0f, 0.0f);

  std::vector<ParticleShader::Particle> shader_particles;
  shader_particles.reserve(particles_.size());
  constexpr float kPartSize = 1.7f;
  for (const auto& part : particles_) {
    if (part.life <= 0.0f) continue;

    shader_particles.push_back({part.pos, part.life * kPartSize, color});
  }

  particle_shader_.Render(model, view, projection, shader_particles);
}

bool Ball::ProcessEvent(const SDL_Event& event) { return false; }

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
