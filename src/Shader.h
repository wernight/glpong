#pragma once

#include <glm/glm.hpp>
#include <string>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

class Shader {
 public:
  Shader(const char* vertex_shader_source, const char* fragment_shader_source);
  ~Shader();

  void Use() const;

  void SetUniform(const std::string& name, const glm::mat4& matrix) const;
  void SetUniform(const std::string& name, const glm::vec3& value) const;
  void SetUniform(const std::string& name, const glm::vec4& value) const;
  void SetUniform(const std::string& name, int value) const;

  GLuint GetAttributeLocation(const std::string& name) const;

 private:
  GLuint program_ = 0;
};
