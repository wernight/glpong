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

#include <SDL2/SDL.h>

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Ball.h"
#include "Board.h"
#include "Shader.h"

constexpr float kPaddleBevel = 4.0f;
constexpr float kPaddleSpeed = 150.0f;
constexpr float kPaddleIlluminate = 0.5f;
constexpr float kPaddleIlluminateFade = 0.3f;

namespace {
const char* kPaddleVertexShader = R"glsl(
#version 300 es
in vec3 aPos;
in vec3 aNormal;

uniform mat4 modelview;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = projection * modelview * vec4(aPos, 1.0);
    FragPos = vec3(modelview * vec4(aPos, 1.0));
    Normal = mat3(modelview) * aNormal;
}
)glsl";

const char* kPaddleFragmentShader = R"glsl(
#version 300 es
precision mediump float;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;

uniform vec3 lightPos;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;

out vec4 FragColor;

void main()
{
    vec3 ambient = lightAmbient * objectColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff * objectColor;

    FragColor = vec4(ambient + diffuse, 1.0);
}
)glsl";

struct Vertex {
  GLfloat position[3];
  GLfloat normal[3];
};

}  // namespace

Paddle::Paddle(bool is_left_paddle)
    : speed_(0.0f), y_(0.0f), illuminate_(0.0f), left_paddle_(is_left_paddle) {
  shader_ = std::make_unique<Shader>(kPaddleVertexShader, kPaddleFragmentShader);

  std::vector<Vertex> vertices;
  if (left_paddle_) {
    vertices.insert(
        vertices.end(),
        {
            // Front face
            {{Board::GetLeft(), GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, 0.0f, -1.0f}},
            {{Board::GetLeft(), -GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, 0.0f, -1.0f}},
            {{Board::GetLeft() - GetWidth(), GetHeight() / 2.0f, -kPaddleBevel},
             {0.0f, 0.0f, -1.0f}},
            {{Board::GetLeft() - GetWidth(), -GetHeight() / 2.0f, -kPaddleBevel},
             {0.0f, 0.0f, -1.0f}},
            // Left face
            {{Board::GetLeft() - GetWidth(), GetHeight() / 2.0f, -kPaddleBevel},
             {-1.0f, 0.0f, 0.0f}},
            {{Board::GetLeft() - GetWidth(), -GetHeight() / 2.0f, -kPaddleBevel},
             {-1.0f, 0.0f, 0.0f}},
            {{Board::GetLeft() - GetWidth(), GetHeight() / 2.0f, 0}, {-1.0f, 0.0f, 0.0f}},
            {{Board::GetLeft() - GetWidth(), -GetHeight() / 2.0f, 0}, {-1.0f, 0.0f, 0.0f}},
            // Bottom face
            {{Board::GetLeft(), -GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, -1.0f, 0.0f}},
            {{Board::GetLeft(), -GetHeight() / 2.0f, 0}, {0.0f, -1.0f, 0.0f}},
            {{Board::GetLeft() - GetWidth(), -GetHeight() / 2.0f, -kPaddleBevel},
             {0.0f, -1.0f, 0.0f}},
            {{Board::GetLeft() - GetWidth(), -GetHeight() / 2.0f, 0}, {0.0f, -1.0f, 0.0f}},
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
            {{Board::GetRight(), GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, 0.0f, -1.0f}},
            {{Board::GetRight(), -GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, 0.0f, -1.0f}},
            // Right face
            {{Board::GetRight() + GetWidth(), GetHeight() / 2.0f, 0}, {1.0f, 0.0f, 0.0f}},
            {{Board::GetRight() + GetWidth(), -GetHeight() / 2.0f, 0}, {1.0f, 0.0f, 0.0f}},
            {{Board::GetRight() + GetWidth(), GetHeight() / 2.0f, -kPaddleBevel},
             {1.0f, 0.0f, 0.0f}},
            {{Board::GetRight() + GetWidth(), -GetHeight() / 2.0f, -kPaddleBevel},
             {1.0f, 0.0f, 0.0f}},
            // Bottom face
            {{Board::GetRight() + GetWidth(), -GetHeight() / 2.0f, -kPaddleBevel},
             {0.0f, -1.0f, 0.0f}},
            {{Board::GetRight() + GetWidth(), -GetHeight() / 2.0f, 0}, {0.0f, -1.0f, 0.0f}},
            {{Board::GetRight(), -GetHeight() / 2.0f, -kPaddleBevel}, {0.0f, -1.0f, 0.0f}},
            {{Board::GetRight(), -GetHeight() / 2.0f, 0}, {0.0f, -1.0f, 0.0f}},
        });
  }
  vertex_count_ = vertices.size();

  shader_->Use();
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);

  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

  GLint posAttrib = shader_->GetAttributeLocation("aPos");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, position));
  GLint normalAttrib = shader_->GetAttributeLocation("aNormal");
  glEnableVertexAttribArray(normalAttrib);
  glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, normal));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Paddle::TrackBall(std::shared_ptr<Ball> ball) { ball_ = ball; }

Paddle::~Paddle() {
  if (vao_) glDeleteVertexArrays(1, &vao_);
  if (vbo_) glDeleteBuffers(1, &vbo_);
}

void Paddle::Update(float dt) {
  total_time_ += dt;
  time_since_last_input_ += dt;

  if (ball_ && time_since_last_input_ > 8.0f) {
    // AI logic from AiPaddle::Update
    glm::vec2 ball_pos = ball_->GetPosition();
    float ball_distance;
    if (left_paddle_)
      ball_distance = Board::GetLeft() - ball_pos.x;
    else
      ball_distance = ball_pos.x - Board::GetRight();

    float rnd = 0.5f * GetHeight() * cos(total_time_ * 6.0f);

    if (ball_distance > 0.75f * Board::GetWidth())
      speed_ = 0.0f;  // Stop()
    else if (y_ < ball_pos.y - 0.2f * GetHeight() + rnd)
      speed_ = kPaddleSpeed;  // MoveUp()
    else if (y_ > ball_pos.y + 0.2f * GetHeight() + rnd)
      speed_ = -kPaddleSpeed;  // MoveDown()
    else
      speed_ = 0.0f;  // Stop()
  }

  // Update paddle position.
  y_ += speed_ * dt;
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
    illuminate_ -= kPaddleIlluminateFade * dt;
  else
    illuminate_ = 0.0f;
}

void Paddle::Render(const glm::mat4& view, const glm::mat4& model,
                    const glm::mat4& projection) const {
  shader_->Use();
  shader_->SetUniform("projection", projection);

  glm::mat4 paddle_modelview = glm::translate(model * view, glm::vec3(0.0f, y_, 0.0f));
  shader_->SetUniform("modelview", paddle_modelview);

  shader_->SetUniform("lightPos", glm::vec3(30.0f, 50.0f, -100.0f));
  shader_->SetUniform("lightAmbient", glm::vec3(0.5f, 0.5f, 0.5f));
  shader_->SetUniform("lightDiffuse", glm::vec3(1.0f, 1.0f, 1.0f));
  shader_->SetUniform("objectColor", glm::vec3(illuminate_, 1.0f, illuminate_));

  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_count_);
  glBindVertexArray(0);
}

bool Paddle::ProcessEvent(const SDL_Event& event) {
  if (left_paddle_) {
    // Left paddle is controlled by A/Q or Ctrl/Shift keys.
    switch (event.type) {
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LSHIFT) {
          MoveUp();
          time_since_last_input_ = 0.0f;
          return true;
        } else if (event.key.keysym.sym == SDLK_q || event.key.keysym.sym == SDLK_LCTRL) {
          MoveDown();
          time_since_last_input_ = 0.0f;
          return true;
        }
        break;
      case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_q ||
            event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_LCTRL) {
          Stop();
          time_since_last_input_ = 0.0f;
          return true;
        }
        break;

      case SDL_FINGERMOTION:
      case SDL_FINGERDOWN: {
        if (event.tfinger.x < 0.5f) {
          time_since_last_input_ = 0.0f;
          // Left half of the screen
          if (event.tfinger.y < 0.5f) {
            // Top half -> move up
            MoveUp();
          } else {
            // Bottom half -> move down
            MoveDown();
          }
        }
        break;
      }
      case SDL_FINGERUP: {
        if (event.tfinger.x < 0.5f) {
          time_since_last_input_ = 0.0f;
          // Left half of the screen
          Stop();
        }
        break;
      }
    }
  } else {
    // Right paddle is controlled by up and down arrow keys.
    switch (event.type) {
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_UP) {
          MoveUp();
          time_since_last_input_ = 0.0f;
          return true;
        } else if (event.key.keysym.sym == SDLK_DOWN) {
          MoveDown();
          time_since_last_input_ = 0.0f;
          return true;
        }
        break;
      case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN) {
          Stop();
          time_since_last_input_ = 0.0f;
          return true;
        }
        break;

      case SDL_FINGERMOTION:
      case SDL_FINGERDOWN: {
        if (event.tfinger.x > 0.5f) {
          time_since_last_input_ = 0.0f;
          // Right half of the screen
          if (event.tfinger.y < 0.5f) {
            // Top half -> move up
            MoveUp();
          } else {
            // Bottom half -> move down
            MoveDown();
          }
        }
        break;
      }
      case SDL_FINGERUP: {
        if (event.tfinger.x > 0.5f) {
          time_since_last_input_ = 0.0f;
          // Right half of the screen
          Stop();
        }
        break;
      }
    }
  }
  return false;
}

void Paddle::MoveUp() { speed_ = kPaddleSpeed; }

void Paddle::MoveDown() { speed_ = -kPaddleSpeed; }

void Paddle::Stop() { speed_ = 0.0f; }

void Paddle::Illuminate() { illuminate_ = kPaddleIlluminate; }
