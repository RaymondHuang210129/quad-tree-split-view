#pragma once
#include <array>

#include <glad/glad.h>
#include <glm/matrix.hpp>

#include "shader.h"

class AxesVaoProvider {
public:
  const GLuint& vao() const;

private:
  mutable GLuint _vao{};
};

class AxesComponent {
public:
  AxesComponent();
  void render(const glm::mat4& view, const glm::mat4& proj) const;

private:
  static inline const BasicShaderProgramProvider shaderProgramProvider{};
  static inline const AxesVaoProvider VaoProvider{};
};
