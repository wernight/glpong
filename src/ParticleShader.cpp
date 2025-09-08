#include "ParticleShader.h"

#include <glm/glm.hpp>
#include <vector>

namespace {
const char* kParticleVertexShader = R"glsl(
#version 130
attribute vec3 aPos;
attribute vec2 aTexCoord;
attribute vec3 aColor;

uniform mat4 modelview;
uniform mat4 projection;

varying vec2 TexCoord;
varying vec3 ParticleColor;

void main()
{
    gl_Position = projection * modelview * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    ParticleColor = aColor;
}
)glsl";

const char* kParticleFragmentShader = R"glsl(
#version 130
varying vec2 TexCoord;
varying vec3 ParticleColor;

uniform sampler2D particleTexture;

void main()
{
    gl_FragColor = texture2D(particleTexture, TexCoord) * vec4(ParticleColor, 1.0);
}
)glsl";
}  // namespace

ParticleShader::ParticleShader(GLuint texture, int max_particle_count)
    : texture_(texture),
      max_particle_count_(max_particle_count),
      shader_(kParticleVertexShader, kParticleFragmentShader) {
  shader_.Use();

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, max_particle_count * 6 * sizeof(ParticleVertex), nullptr,
               GL_DYNAMIC_DRAW);
  GLint posAttrib = shader_.GetAttributeLocation("aPos");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
                        (void*)offsetof(ParticleVertex, position));
  GLint texAttrib = shader_.GetAttributeLocation("aTexCoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
                        (void*)offsetof(ParticleVertex, texcoord));
  GLint colorAttrib = shader_.GetAttributeLocation("aColor");
  glEnableVertexAttribArray(colorAttrib);
  glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
                        (void*)offsetof(ParticleVertex, color));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

ParticleShader::~ParticleShader() {
  if (vao_) glDeleteVertexArrays(1, &vao_);
  if (vbo_) glDeleteBuffers(1, &vbo_);
}

void ParticleShader::Render(const glm::mat4& model, const glm::mat4& view,
                            const glm::mat4& projection,
                            const std::vector<Particle>& particles) const {
  assert(particles.size() <= max_particle_count_);
  if (particles.empty()) return;

  // Get camera vectors for billboarding from the actual view matrix
  const glm::vec3 right(view[0][0], view[1][0], view[2][0]);
  const glm::vec3 up(view[0][1], view[1][1], view[2][1]);

  std::vector<ParticleVertex> vertices;
  vertices.reserve(particles.size() * 6);
  for (const auto& part : particles) {
    const glm::vec3 center = part.center;
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

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glDisable(GL_DEPTH_TEST);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);

  shader_.Use();
  shader_.SetUniform("projection", projection);
  shader_.SetUniform("modelview", model * view);
  shader_.SetUniform("particleTexture", 0);

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(ParticleVertex), vertices.data());
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  glBindVertexArray(0);

  glEnable(GL_DEPTH_TEST);
}
