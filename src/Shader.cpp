#include "Shader.h"

#include <iostream>
#include <vector>

static GLuint CompileShader(GLenum type, const char* source) {
  // Skip leading line breaks.
  while (*source == '\n') {
    ++source;
  }

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<char> log(log_length);
    glGetShaderInfoLog(shader, log_length, nullptr, log.data());
    std::cout << "Shader code: " << std::endl << source << std::endl;
    std::cerr << "Shader compilation failed: " << log.data() << std::endl;
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

Shader::Shader(const char* vertex_shader_source, const char* fragment_shader_source) {
  GLuint vertex_shader = CompileShader(GL_VERTEX_SHADER, vertex_shader_source);
  GLuint fragment_shader = CompileShader(GL_FRAGMENT_SHADER, fragment_shader_source);

  program_ = glCreateProgram();
  glAttachShader(program_, vertex_shader);
  glAttachShader(program_, fragment_shader);
  glLinkProgram(program_);

  GLint success;
  glGetProgramiv(program_, GL_LINK_STATUS, &success);
  if (!success) {
    GLint log_length;
    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<char> log(log_length);
    glGetProgramInfoLog(program_, log_length, nullptr, log.data());
    std::cerr << "Shader linking failed: " << log.data() << std::endl;
    glDeleteProgram(program_);
    program_ = 0;
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

Shader::~Shader() {
  if (program_) {
    glDeleteProgram(program_);
  }
}

void Shader::Use() const { glUseProgram(program_); }

void Shader::SetUniform(const std::string& name, const glm::mat4& matrix) const {
  glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::SetUniform(const std::string& name, const glm::vec3& value) const {
  glUniform3fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
}

void Shader::SetUniform(const std::string& name, const glm::vec4& value) const {
  glUniform4fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
}

void Shader::SetUniform(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(program_, name.c_str()), value);
}

GLuint Shader::GetAttributeLocation(const std::string& name) const {
  return glGetAttribLocation(program_, name.c_str());
}
