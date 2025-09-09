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
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <random>

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
// Random number in [a, b]
int RandomClosedInt(int a, int b) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(a, b);
  return dist(gen);
}

// Random number in [a, b)
float RandomRange(float a, float b) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(a, b);
  return dist(gen);
}

// Random number in [a, b]
float RandomClosedRange(float a, float b) {
  return RandomRange(a, std::nextafter(b, std::numeric_limits<float>::max()));
}

// Random number of value (a) with a stdvar of (b).
float RandomApprox(float a, float b) { return a + b * RandomClosedRange(-0.5f, 0.5f); }
}  // namespace

FireworkRocket::FireworkRocket() : is_exploding_(false) { Create(); }

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
  float angle = M_PI / 2.0f + RandomClosedRange(-M_PI / 4.0f, M_PI / 4.0f);
  constexpr float kSpeedFactor = 13.0f;
  part_rocket_.speed.x = cos(angle) * kSpeedFactor;
  part_rocket_.speed.y = sin(angle) * kSpeedFactor;
  part_rocket_.speed.z = sin(RandomClosedRange(-M_PI / 8.0f, M_PI / 8.0f)) * kSpeedFactor;
  part_rocket_.pos.x = -30.0f + angle * 60.0f / M_PI + RandomClosedRange(-10.0f, 10.0f);
  part_rocket_.pos.y = RandomClosedRange(-24.0f, -15.0f);
  part_rocket_.pos.z = 0.0f;
  part_rocket_.color = part_rocket_.ini_color = {1.0f, 0.8f, 0.2f};

  // Rocket's sparks
  for (auto& part : part_spark_) CreateRocketSpark(part);

  // Explosion's pink
  int exposition_color = RandomClosedInt(0, kColorCount - 1);
  for (int i = 0; i < part_pink_.size(); ++i) {
    Particle& part = part_pink_[i];
    part = {};
    part.active = false;
    part.life = part.ini_life = RandomApprox(1.6f, 1.9f);
    part.ini_size = 0.8f;
    part.weight = 1.0f * part.ini_life;
    angle = RandomRange(0, 2.0 * M_PI);
    float angle2 = RandomRange(0, 2.0 * M_PI);
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

void FireworkRocket::CreateRocketSpark(Particle& particle) {
  int color = RandomClosedInt(0, kColorCount2 - 1);
  float rnd = RandomRange(0.0f, 0.1f);

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

void FireworkRocket::AddParticles(std::vector<ParticleShader::Particle>& shader_particles) const {
  // Get camera vectors for billboarding from the actual view matrix
  auto add_particles = [&](const auto& particles) {
    shader_particles.reserve(shader_particles.size() + particles.size());
    for (const auto& part : particles) {
      if (!part.active) continue;

      shader_particles.push_back({part.pos, part.size, part.color});
    }
  };

  // Render Rocket's sparks
  add_particles(part_spark_);

  // Render explosion
  if (is_exploding_) {
    add_particles(part_pink_);
    add_particles(part_fire_);
  }
}

void FireworkRocket::Update(float dt) {
  static float t = 0.0f;
  float life;
  float alpha;

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

  if (!is_exploding_) return;

  // Update explosion particles
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

  bool end_explode = true;
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

  // Re-create a new rocket once the explosion is complete.
  if (end_explode) Create();
}

void FireworkRocket::Explode() {
  for (auto& part : part_pink_) {
    part.active = true;
    part.pos = part_rocket_.pos;
  }

  is_exploding_ = true;
}

Firework::Firework(GLuint texture, int rocket_count)
    : particle_shader_(texture, rocket_count * FireworkRocket::MaxParticles()),
      rockets_(rocket_count) {}

void Firework::Update(float dt) {
  for (auto& rocket : rockets_) {
    rocket.Update(dt);
  }
}

void Firework::Render(const glm::mat4& view_ignored, const glm::mat4& model_ignored,
                      const glm::mat4& projection_ignored) const {
  // We render the firework on top regardless of the camera view used during the game.
  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 120.0f));
  glm::mat4 view = glm::lookAt(glm::vec3(0.0f, -120.0f, -100.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 projection = glm::perspective(glm::radians(45.0f),  // field of view in radians
                                          4.0f / 3.0f,          // width/height of viewport
                                          0.1f,                 // near plane
                                          1000.0f);             // far plane

  std::vector<ParticleShader::Particle> particles;
  for (const auto& rocket : rockets_) {
    rocket.AddParticles(particles);
  }

  particle_shader_.Render(model, view, projection, particles);
}

bool Firework::ProcessEvent(const SDL_Event& event) {
  // Any key press or touch event will skip the firework animation.
  if (event.type == SDL_KEYDOWN || event.type == SDL_FINGERDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
    is_done_ = true;
    return true;
  }
  return false;
}
