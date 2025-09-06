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
#include <vector>

#include "StdAfx.h"
#include "Vector.h"

constexpr int kColorCount = 12;
constexpr int kColorCount2 = 6;

using ColorVec3 = std::array<GLfloat, 3>;

// Rainbow of kColorCount
constexpr std::array<ColorVec3, kColorCount> kRainbowkColorCount = {{{1.0f, 0.10f, 0.10f},
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
constexpr std::array<ColorVec3, kColorCount> kWarmkColorCount = {{{1.0f, 0.5f, 0.0f},
                                                                  {1.0f, 0.75f, 0.5f},
                                                                  {1.0f, 1.0f, 0.7f},
                                                                  {1.0f, 1.0f, 0.5f},
                                                                  {0.8f, 0.8f, 0.8f},
                                                                  {1.0f, 1.0f, 0.8f}}};

namespace {
struct ParticleVertex {
  GLfloat position[3];
  GLfloat texcoord[2];
  GLfloat color[3];
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
  part_rocket_.xi = (float)cos(angle) * 13.0f;  // X Axis Speed And Direction
  part_rocket_.yi = (float)sin(angle) * 13.0f;  // Y Axis Speed And Direction
  part_rocket_.zi = (float)sin(rand_.RandomRange(-M_PI / 8.0f, M_PI / 8.0f)) *
                    13.0f;  // Z Axis Speed And Direction
  part_rocket_.x = float(-30.0f + angle * 60.0f / M_PI + rand_.RandomRange(-10.0, 10.0));
  part_rocket_.y = (float)rand_.RandomRange(-24.0, -15.0);
  part_rocket_.z = 0.0f;
  part_rocket_.color[0] = part_rocket_.ini_color[0] = 1.0f;
  part_rocket_.color[1] = part_rocket_.ini_color[1] = 0.8f;
  part_rocket_.color[2] = part_rocket_.ini_color[2] = 0.2f;

  // Rocket's sparks
  for (auto& part : part_spark_) CreateRocketSpark(part);

  // Explosion's pink
  int exposition_color = rand_.RandomInt() % kColorCount;
  for (int i = 0; i < part_pink_.size(); ++i) {
    Particle& part = part_pink_[i];
    memset(&part, 0, sizeof(Particle));
    part.active = false;
    part.life = part.ini_life = RandomApprox(1.6f, 1.9f);
    part.ini_size = 0.8f;
    part.weight = 1.0f * part.ini_life;
    angle = float(rand_.RandomReal2() * 2.0 * M_PI);
    float angle2 = float(rand_.RandomReal2() * 2.0 * M_PI);
    float velocity = RandomApprox(7.2f, 11.1f);
    part.xi = velocity * float(cos(angle2) * cos(angle));
    part.yi = velocity * float(cos(angle2) * sin(angle));
    part.zi = velocity * float(sin(angle2));
    part.color[0] = part.ini_color[0] = kRainbowkColorCount[exposition_color][0];
    part.color[1] = part.ini_color[1] = kRainbowkColorCount[exposition_color][1];
    part.color[2] = part.ini_color[2] = kRainbowkColorCount[exposition_color][2];

    // Explosion's fire (trail following the pink bulbs).
    for (int j = 0; j < kExplosionFireCount; ++j) {
      memcpy(&part_fire_[i * kExplosionFireCount + j], &part, sizeof(Particle));
      part_fire_[i * kExplosionFireCount + j].weight = 0.8f;
      part_fire_[i * kExplosionFireCount + j].xi = 0.0f;
      part_fire_[i * kExplosionFireCount + j].yi = 0.0f;
      part_fire_[i * kExplosionFireCount + j].zi = 0.0f;
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
  particle.x = RandomApprox(part_rocket_.x, 0.1f) - rnd * part_rocket_.xi;
  particle.y = RandomApprox(part_rocket_.y, 0.1f) - rnd * part_rocket_.yi;
  particle.z = RandomApprox(part_rocket_.z, 0.1f) - rnd * part_rocket_.zi;
  particle.xi = RandomApprox(0.0f, 0.8f);
  particle.yi = RandomApprox(0.0f, 0.8f);
  particle.zi = RandomApprox(0.0f, 0.8f);
  particle.color[0] = particle.ini_color[0] = kWarmkColorCount[color][0];
  particle.color[1] = particle.ini_color[1] = kWarmkColorCount[color][1];
  particle.color[2] = particle.ini_color[2] = kWarmkColorCount[color][2];
}

bool FireworkRocket::Render() const {
  // Get camera vectors for billboarding
  float modelview[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
  const Vector3D right(modelview[0], modelview[4], modelview[8]);
  const Vector3D up(modelview[1], modelview[5], modelview[9]);

  auto fill_vertices = [&](std::vector<ParticleVertex>& vertices, const auto& particles) {
    vertices.clear();
    vertices.reserve(particles.size() * 6);
    for (const auto& part : particles) {
      if (!part.active) continue;
      const Vector3D center(part.x, part.y, part.z);
      const float size = part.size;
      const float* color = part.color;

      const Vector3D tl = center + (-right + up) * size;
      const Vector3D tr = center + (right + up) * size;
      const Vector3D bl = center + (-right - up) * size;
      const Vector3D br = center + (right - up) * size;

      vertices.push_back({{tl.x, tl.y, tl.z}, {0, 1}, {color[0], color[1], color[2]}});
      vertices.push_back({{bl.x, bl.y, bl.z}, {0, 0}, {color[0], color[1], color[2]}});
      vertices.push_back({{tr.x, tr.y, tr.z}, {1, 1}, {color[0], color[1], color[2]}});

      vertices.push_back({{tr.x, tr.y, tr.z}, {1, 1}, {color[0], color[1], color[2]}});
      vertices.push_back({{bl.x, bl.y, bl.z}, {0, 0}, {color[0], color[1], color[2]}});
      vertices.push_back({{br.x, br.y, br.z}, {1, 0}, {color[0], color[1], color[2]}});
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
    part_rocket_.x += part_rocket_.xi * dt;
    part_rocket_.y += part_rocket_.yi * dt;
    part_rocket_.z += part_rocket_.zi * dt;

    if (part_rocket_.life > 0.0f) {
      life = part_rocket_.life / part_rocket_.ini_life;

      part_rocket_.xi += 0.03f * sin(t * part_rocket_.ini_life * 4.0f);
      part_rocket_.yi += 0.03f * cos(t * part_rocket_.ini_life * 2.0f) - part_rocket_.weight * dt;
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

    part.x += part.xi * life * life * dt;  // Move On The X Axis By X Speed
    part.y += part.yi * life * life * dt;  // Move On The Y Axis By Y Speed
    part.z += part.zi * life * life * dt;  // Move On The Z Axis By Z Speed

    part.yi -= float(part.weight * dt);
    part.size = part.ini_size * life * life;
    if (life < 0.2f) {
      alpha = life * 5.0f;
      part.color[0] = part.ini_color[0] * alpha;
      part.color[1] = part.ini_color[1] * alpha;
      part.color[2] = part.ini_color[2] * alpha;
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

      part_pink.x += part_pink.xi * life * life * dt;
      part_pink.y += part_pink.yi * life * life * dt;
      part_pink.z += part_pink.zi * life * life * dt;

      part_pink.yi -= part_pink.weight * dt;
      part_pink.size = part_pink.ini_size * life * life;
      if (life < 0.2f) {
        alpha = life * 5.0f;
        part_pink.color[0] = part_pink.ini_color[0] * alpha;
        part_pink.color[1] = part_pink.ini_color[1] * alpha;
        part_pink.color[2] = part_pink.ini_color[2] * alpha;
      }

      part_pink.life -= (float)dt;
      if (part_pink.life < 0.0f) part_pink.active = false;

      // Create fire trail
      int j = int(life * (kExplosionFireCount - 2) + 1 + cos(t * 10.0f));
      Particle& part_fire = part_fire_[i * kExplosionFireCount + j];
      if (!part_fire.active) {
        part_fire.active = true;
        part_fire.life = 0.7f * part_pink.life;
        part_fire.x = part_pink.x;
        part_fire.y = part_pink.y;
        part_fire.z = part_pink.z;
      }
    }

    for (auto& part : part_fire_) {
      if (!part.active) continue;

      end_explode = false;

      life = part.life / part.ini_life;

      part.x += part.xi * dt;
      part.y += part.yi * dt;
      part.z += part.zi * dt;

      part.yi -= part.weight * dt;
      part.size = part.ini_size * life * life;
      if (life < 0.2f) {
        alpha = life * 5.0f;
        part.color[0] = part.ini_color[0] * alpha;
        part.color[1] = part.ini_color[1] * alpha;
        part.color[2] = part.ini_color[2] * alpha;
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
    part.x = part_rocket_.x;
    part.y = part_rocket_.y;
    part.z = part_rocket_.z;
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
  glEnableClientState(GL_COLOR_ARRAY);
  glTranslatef(0.0f, 0.0f, -40.0f);
  for (const FireworkRocket& rocket : rockets_) rocket.Render();

  glDisableClientState(GL_COLOR_ARRAY);
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