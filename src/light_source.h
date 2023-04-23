#pragma once
#define _USE_MATH_DEFINES

#include <array>
#include <cmath>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "sphere.h"

class LightSourceVaoProvider {
public:
  const std::vector<glm::vec3> vertices{tessellateIcosahedron(3)};
  const GLuint& vao() const;

private:
  mutable GLuint _vao{};
};

class LightSourceComponent {
public:
  LightSourceComponent(const glm::vec3& position);
  void render(const glm::mat4& view, const glm::mat4& proj) const;
  const glm::vec3& position() const;

private:
  static inline const BasicShaderProgramProvider shaderProgramProvider{};
  static inline const LightSourceVaoProvider vaoProvider{};

  glm::vec3 _position{0.0};
  glm::mat4 model{1.0};
  const glm::vec4 _color{1.0f};
};
