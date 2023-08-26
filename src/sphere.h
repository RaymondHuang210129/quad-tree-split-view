#pragma once
#define _USE_MATH_DEFINES

#include <array>
#include <cmath>
#include <random>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "user_control.h"

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

struct AnimatedSphereDataV2 {
  glm::vec3 originalPosition;
  glm::vec3 movingScale;
  float cycleOffset;
  SphereData sphereData;
  float speed;
};

class SphereComponent {
public:
  SphereComponent();
  SphereComponent(AnimatedSphereDataV2 data);
  void render(const glm::mat4& view, const glm::mat4& proj,
              const SphereData& data, const glm::vec3& viewPosition,
              const glm::vec3& lightPosition) const;
  void render(const glm::mat4& view, const glm::mat4& proj,
              const glm::vec3& viewPosition,
              const glm::vec3& lightPosition) const;
  void updatePosition(const double& currentTimestamp);

private:
  static inline const LightingShaderProgramProvider shaderProgramProvider{};
  static inline const SphereVaoProvider vaoProvider{};
  struct AnimatedSphereDataV2 data {};
};

AnimatedSphereDataV2 generateRandomAnimatedSphereDataV2();
