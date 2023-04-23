#pragma once
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

static const GLuint
buildShaderProgram(const std::unordered_map<std::string, GLenum>& sourceFiles);

class BasicShaderProgramProvider {
public:
  const GLuint& program() const;

private:
  mutable GLuint _program{};
};

class LightingShaderProgramProvider {
public:
  const GLuint& program() const;

private:
  mutable GLuint _program{};
};

class TextureLightingShaderProgramProvider {
public:
  const GLuint& program() const;

private:
  mutable GLuint _program{};
};

template <typename T>
concept UniformAcceptable =
    std::is_same_v<T, glm::mat4> || std::is_same_v<T, glm::vec4> ||
    std::is_same_v<T, glm::vec3> || std::is_same_v<T, GLfloat>;

template <UniformAcceptable T>
void setUniformToProgram(const GLuint& shaderProgram, const std::string& name,
                         const T& data) {
  glUseProgram(shaderProgram);
  if constexpr (std::is_same_v<T, glm::mat4>) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                       GL_FALSE, glm::value_ptr(data));
  } else if constexpr (std::is_same_v<T, glm::vec4>) {
    glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                 glm::value_ptr(data));
  } else if constexpr (std::is_same_v<T, glm::vec3>) {
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                 glm::value_ptr(data));
  } else if constexpr (std::is_same_v<T, GLfloat>) {
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), data);
  }
}
