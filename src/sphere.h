#pragma once
#define _USE_MATH_DEFINES

#include <array>
#include <cmath>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

const std::vector<glm::vec3> tessellateIcosahedron(const size_t& divisionCount);

class SphereVaoProvider {
public:
  const std::vector<glm::vec3> vertices{tessellateIcosahedron(3)};
  const GLuint& vao() const;

private:
  mutable GLuint _vao{};
};

struct SphereData {
  glm::vec3 position;
  glm::vec3 color;
};

class SphereComponent {
public:
  SphereComponent();
  void render(const glm::mat4& view, const glm::mat4& proj,
              const SphereData& data, const glm::vec3& viewPosition,
              const glm::vec3& lightPosition) const;

private:
  static inline const LightingShaderProgramProvider shaderProgramProvider{};
  static inline const SphereVaoProvider vaoProvider{};
};
