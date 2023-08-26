#pragma once
#include <array>
#include <cstddef>
#include <string>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "user_control.h"

static constexpr int numSurfaces = 10;
static constexpr int numPointsOfSurface = 4;

class CameraVaoProvider {
public:
  const GLuint& vao() const;

private:
  mutable GLuint _vao{};
};

class CameraComponent {
public:
  CameraComponent(const glm::vec3& position);
  void render(const glm::mat4& view, const glm::mat4& proj,
              double horizontalAngleRadians, double verticalAngleRadians,
              const glm::vec3& viewPosition,
              const glm::vec3& lightPosition) const;
  // void render(const glm::mat4& view, const glm::mat4& proj,
  //             const glm::vec3& viewPosition,
  //             const glm::vec3& lightPosition) const;
  static void updateUserControlData(const UserControlData& userData);

private:
  static inline const LightingShaderProgramProvider shaderProgramProvider{};
  static inline const CameraVaoProvider vaoProvider{};
  mutable glm::mat4 model{1.0};
  static UserControlData userData;
};
