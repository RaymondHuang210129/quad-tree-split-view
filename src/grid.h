#pragma once
#include <vector>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

class GridVaoProvider {
public:
  const GLsizei gridCount{20};
  const GLuint& vao() const;

private:
  mutable GLuint _vao{};
};

class GridComponent {
public:
  GridComponent();
  void render(const glm::mat4& view, const glm::mat4& proj) const;

private:
  static inline const BasicShaderProgramProvider shaderProgramProvider{};
  static inline const GridVaoProvider vaoProvider{};
};
