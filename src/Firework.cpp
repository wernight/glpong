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
 *
 * Firework class implementation file.
 *
 * @name Firework
 * @version 1.1
 * @date 2003.11.17
 */

/**
 * Copyright (c) 2003 Werner BEROUX
 * Mail: werner@beroux.com
 * Web : www.beroux.com
 */

#include "Firework.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "RandomMT.h"
#include "StdAfx.h"

constexpr int kColorCount = 12;
constexpr int kColorCount2 = 6;

// Rainbow of kColorCount
constexpr std::array<glm::vec3, kColorCount> kRainbowkColorCount = {{{1.0f, 0.10f, 0.10f},
                                                                     {1.0f, 0.50f, 0.30f},
                                                                     {1.0f, 1.0f, 0.50f},
                                                                     {0.50f, 1.0f, 0.25f},
                                                                     {0.25f, 1.0f, 0.25f},
                                                                     {0.25f, 1.0f, 0.50f},
                                                                     {0.25f, 1.0f, 1.0f},
                                                                     {0.25f, 0.50f, 1.0f},
                                                                     {0.25f, 0.25f, 1.0f},
                                                                     {0.50f, 0.25f, 1.0f},
                                                                     {1.0f, 0.25f, 1.0f},
                                                                     {1.0f, 0.25f, 0.50f}}};

// Yellow/Orange/Bright kColorCount
constexpr std::array<glm::vec3, kColorCount2> kWarmkColorCount = {{{1.0f, 0.5f, 0.0f},
                                                                   {1.0f, 0.75f, 0.5f},
                                                                   {1.0f, 1.0f, 0.7f},
                                                                   {1.0f, 1.0f, 0.5f},
                                                                   {0.8f, 0.8f, 0.8f},
                                                                   {1.0f, 1.0f, 0.8f}}};

namespace {
struct ParticleVertex {
  glm::vec3 position;
  glm::vec2 texcoord;
  glm::vec3 color;
};
}  // namespace

RandomMT FireworkRocket::rand_;

FireworkRocket::FireworkRocket() : is_exploding_(false) {
  Create();

  // Sparks
  glGenVertexArrays(1, &spark_vao_);
  glGenBuffers(1, &spark_vbo_);
  glBindVertexArray(spark_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, spark_vbo_);
  glBufferData(GL_ARRAY_BUFFER, part_spark_.size() * 6 * sizeof(ParticleVertex), nullptr,
               GL_DYNAMIC_DRAW);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, position));
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, texcoord));
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(3, GL_FLOAT, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, color));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Pink
  glGenVertexArrays(1, &pink_vao_);
  glGenBuffers(1, &pink_vbo_);
  glBindVertexArray(pink_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, pink_vbo_);
  glBufferData(GL_ARRAY_BUFFER, part_pink_.size() * 6 * sizeof(ParticleVertex), nullptr,
               GL_DYNAMIC_DRAW);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, position));
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, texcoord));
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(3, GL_FLOAT, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, color));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Fire
  glGenVertexArrays(1, &fire_vao_);
  glGenBuffers(1, &fire_vbo_);
  glBindVertexArray(fire_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, fire_vbo_);
  glBufferData(GL_ARRAY_BUFFER, part_fire_.size() * 6 * sizeof(ParticleVertex), nullptr,
               GL_DYNAMIC_DRAW);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, position));
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, texcoord));
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(3, GL_FLOAT, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, color));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void FireworkRocket::Create() {
  // Create particles
  for (auto& part : part_pink_) part.active = false;
  for (auto& part : part_fire_) part.active = false;
  is_exploding_ = false;

  // Rocket's particle
  part_rocket_.active = true;
  part_rocket_.life = part_rocket_.ini_life = RandomApprox(2.5f, 3.0f);
  part_rocket_.ini_size = 0.5f;
  part_rocket_.weight = 2.0f;
  float angle = float(M_PI) / 2.0f + (float)rand_.RandomRange(-M_PI / 4.0f, M_PI / 4.0f);
  part_rocket_.speed.x = (float)cos(angle) * 13.0f;  // X Axis Speed And Direction
  part_rocket_.speed.y = (float)sin(angle) * 13.0f;  // Y Axis Speed And Direction
  part_rocket_.speed.z = (float)sin(rand_.RandomRange(-M_PI / 8.0f, M_PI / 8.0f)) *
                         13.0f;  // Z Axis Speed And Direction
  part_rocket_.pos.x = float(-30.0f + angle * 60.0f / M_PI + rand_.RandomRange(-10.0, 10.0));
  part_rocket_.pos.y = (float)rand_.RandomRange(-24.0, -15.0);
  part_rocket_.pos.z = 0.0f;
  part_rocket_.color = part_rocket_.ini_color = {1.0f, 0.8f, 0.2f};

  // Rocket's sparks
  for (auto& part : part_spark_) CreateRocketSpark(part);

  // Explosion's pink
  int exposition_color = rand_.RandomInt() % kColorCount;
  for (int i = 0; i < part_pink_.size(); ++i) {
    Particle& part = part_pink_[i];
    part = {};
    part.active = false;
    part.life = part.ini_life = RandomApprox(1.6f, 1.9f);
    part.ini_size = 0.8f;
    part.weight = 1.0f * part.ini_life;
    angle = float(rand_.RandomReal2() * 2.0 * M_PI);
    float angle2 = float(rand_.RandomReal2() * 2.0 * M_PI);
    float velocity = RandomApprox(7.2f, 11.1f);
    part.speed.x = velocity * float(cos(angle2) * cos(angle));
    part.speed.y = velocity * float(cos(angle2) * sin(angle));
    part.speed.z = velocity * float(sin(angle2));
    part.color = part.ini_color = kRainbowkColorCount[exposition_color];

    // Explosion's fire (trail following the pink bulbs).
    for (int j = 0; j < kExplosionFireCount; ++j) {
      part_fire_[i * kExplosionFireCount + j] = part;
      part_fire_[i * kExplosionFireCount + j].weight = 0.8f;
      part_fire_[i * kExplosionFireCount + j].speed = {0.0f, 0.0f, 0.0f};
      part_fire_[i * kExplosionFireCount + j].color =
          part_fire_[i * kExplosionFireCount + j].ini_color = {1.0f, 0.5f, 0.0f};
    }
  }
}

FireworkRocket::~FireworkRocket() {
  if (spark_vao_) glDeleteVertexArrays(1, &spark_vao_);
  if (spark_vbo_) glDeleteBuffers(1, &spark_vbo_);
  if (pink_vao_) glDeleteVertexArrays(1, &pink_vao_);
  if (pink_vbo_) glDeleteBuffers(1, &pink_vbo_);
  if (fire_vao_) glDeleteVertexArrays(1, &fire_vao_);
  if (fire_vbo_) glDeleteBuffers(1, &fire_vbo_);
}

float FireworkRocket::RandomApprox(float a, float b) {
  // Random number of value (a) with a variation of (b).
  return a + (rand_.RandomReal1() - 0.5f) * b;
}

void FireworkRocket::CreateRocketSpark(Particle& particle) {
  int color = rand_.RandomInt() % kColorCount2;
  float rnd = float(rand_.RandomReal2()) * 0.1f;

  particle.active = true;
  particle.life = particle.ini_life = RandomApprox(1.1f, 2.0f);
  particle.ini_size = RandomApprox(0.2f, 0.3f);
  particle.weight = 0.5f;
  particle.pos = part_rocket_.pos - (rnd * part_rocket_.speed +
                                     glm::vec3{RandomApprox(0.0f, 0.1f), RandomApprox(0.0f, 0.1f),
                                               RandomApprox(0.0f, 0.1f)});
  particle.speed = {RandomApprox(0.0f, 0.8f), RandomApprox(0.0f, 0.8f), RandomApprox(0.0f, 0.8f)};
  particle.color = particle.ini_color = kWarmkColorCount[color];
}

bool FireworkRocket::Render() const {
  // Get camera vectors for billboarding
  glm::mat4 modelview;
  glGetFloatv(GL_MODELVIEW_MATRIX, &modelview[0][0]);
  const glm::vec3 right(modelview[0][0], modelview[1][0], modelview[2][0]);
  const glm::vec3 up(modelview[0][1], modelview[1][1], modelview[2][1]);

  auto fill_vertices = [&](std::vector<ParticleVertex>& vertices, const auto& particles) {
    vertices.clear();
    vertices.reserve(particles.size() * 6);
    for (const auto& part : particles) {
      if (!part.active) continue;
      const glm::vec3 center = part.pos;
      const float size = part.size;
      const glm::vec3 color = part.color;

      const glm::vec3 tl = center + (-right + up) * size;
      const glm::vec3 tr = center + (right + up) * size;
      const glm::vec3 bl = center + (-right - up) * size;
      const glm::vec3 br = center + (right - up) * size;

      vertices.push_back({tl, {0, 1}, color});
      vertices.push_back({bl, {0, 0}, color});
      vertices.push_back({tr, {1, 1}, color});

      vertices.push_back({tr, {1, 1}, color});
      vertices.push_back({bl, {0, 0}, color});
      vertices.push_back({br, {1, 0}, color});
    }
  };

  std::vector<ParticleVertex> vertices;

  // Render Rocket's sparks
  fill_vertices(vertices, part_spark_);
  if (!vertices.empty()) {
    glBindVertexArray(spark_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, spark_vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(ParticleVertex), vertices.data());
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  }

  // Render explosion
  if (is_exploding_) {
    fill_vertices(vertices, part_pink_);
    if (!vertices.empty()) {
      glBindVertexArray(pink_vao_);
      glBindBuffer(GL_ARRAY_BUFFER, pink_vbo_);
      glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(ParticleVertex),
                      vertices.data());
      glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }

    fill_vertices(vertices, part_fire_);
    if (!vertices.empty()) {
      glBindVertexArray(fire_vao_);
      glBindBuffer(GL_ARRAY_BUFFER, fire_vbo_);
      glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(ParticleVertex),
                      vertices.data());
      glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
  }
  glBindVertexArray(0);

  return true;
}

bool FireworkRocket::Update(float dt) {
  static float t = 0.0f;
  float life;
  float alpha;
  bool end_explode = true;

  t += dt;

  // Update rocket
  if (part_rocket_.active) {
    part_rocket_.pos += part_rocket_.speed * dt;

    if (part_rocket_.life > 0.0f) {
      life = part_rocket_.life / part_rocket_.ini_life;

      part_rocket_.speed.x += 0.03f * sin(t * part_rocket_.ini_life * 4.0f);
      part_rocket_.speed.y +=
          0.03f * cos(t * part_rocket_.ini_life * 2.0f) - part_rocket_.weight * dt;
    }

    part_rocket_.life -= dt;
    if (part_rocket_.life < -0.3f) {
      part_rocket_.active = false;
      Explode();
    }
  }

  // Update sparks
  for (int i = 0; i < part_spark_.size(); ++i) {
    Particle& part = part_spark_[i];
    if (!part.active) continue;

    life = part.life / part.ini_life;

    part.pos += part.speed * life * life * dt;

    part.speed.y -= float(part.weight * dt);
    part.size = part.ini_size * life * life;
    if (life < 0.2f) {
      alpha = life * 5.0f;
      part.color = part.ini_color * alpha;
    }

    part.life -= dt;
    if (part.life < 0.0f) {
      // If Particle Is Burned Out
      if (part_rocket_.life < float(i) / kRocketFireCount)
        part.active = false;
      else
        CreateRocketSpark(part);
    }
  }

  // Update explosion particles
  if (is_exploding_) {
    for (int i = 0; i < part_pink_.size(); ++i) {
      Particle& part_pink = part_pink_[i];
      if (!part_pink.active) continue;

      life = part_pink.life / part_pink.ini_life;

      part_pink.pos += part_pink.speed * life * life * dt;

      part_pink.speed.y -= part_pink.weight * dt;
      part_pink.size = part_pink.ini_size * life * life;
      if (life < 0.2f) {
        alpha = life * 5.0f;
        part_pink.color = part_pink.ini_color * alpha;
      }

      part_pink.life -= (float)dt;
      if (part_pink.life < 0.0f) part_pink.active = false;

      // Create fire trail
      int j = int(life * (kExplosionFireCount - 2) + 1 + cos(t * 10.0f));
      Particle& part_fire = part_fire_[i * kExplosionFireCount + j];
      if (!part_fire.active) {
        part_fire.active = true;
        part_fire.life = 0.7f * part_pink.life;
        part_fire.pos = part_pink.pos;
      }
    }

    for (auto& part : part_fire_) {
      if (!part.active) continue;

      end_explode = false;

      life = part.life / part.ini_life;

      part.pos += part.speed * dt;

      part.speed.y -= part.weight * dt;
      part.size = part.ini_size * life * life;
      if (life < 0.2f) {
        alpha = life * 5.0f;
        part.color = part.ini_color * alpha;
      }

      part.life -= dt;
      if (part.life < 0.0f)  // If Particle Is Burned Out
        part.active = false;
    }

    if (end_explode) Create();  // Re-create a new rocket.
  }
  return true;
}

void FireworkRocket::Explode() {
  for (auto& part : part_pink_) {
    part.active = true;
    part.pos = part_rocket_.pos;
  }

  is_exploding_ = true;
}

Firework::Firework(GLuint texture) : texture_(texture) {}

void Firework::Update(float dt) {
  for (FireworkRocket& rocket : rockets_) rocket.Update(dt);
}

void Firework::Render() const {
  // Init GL
  glBlendFunc(GL_ONE, GL_ONE);                                // Type Of Blending To Perform
  if (texture_ != 0) glBindTexture(GL_TEXTURE_2D, texture_);  // Select Our Texture

  // Draw fireword
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glTranslatef(0.0f, 0.0f, -40.0f);
  for (const FireworkRocket& rocket : rockets_) rocket.Render();

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glEnable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
}

bool Firework::ProcessEvent(EEvent nEvent, unsigned long wParam, unsigned long lParam) {
  if (nEvent == eventKeyDown) {
    is_done_ = true;
    return true;
  }
  return false;
}