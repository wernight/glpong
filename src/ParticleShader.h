#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Shader.h"

class ParticleShader {
 public:
  struct Particle {
    glm::vec3 center;
    float size;
    glm::vec3 color;
  };

  ParticleShader(GLuint texture, int max_particle_count);

  ~ParticleShader();

  void Render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection,
              const std::vector<Particle>& particle) const;

 private:
  struct ParticleVertex {
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 color;
  };

  GLuint texture_;
  int max_particle_count_;
  Shader shader_;
  GLuint vao_ = 0;
  GLuint vbo_ = 0;
};
