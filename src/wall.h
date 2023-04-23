#pragma once
#include <array>
#include <cstddef>
#include <string>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"

class WallVaoProvider {
public:
  const GLuint& vao() const;

private:
  mutable GLuint _vao{};
};

class WallComponent {
public:
  WallComponent(const glm::vec3& position, const bool& rotate90Deg);
  void render(const glm::mat4& view, const glm::mat4& proj,
              const glm::vec3& viewPosition,
              const glm::vec3& lightPosition) const;

private:
  static inline const TextureLightingShaderProgramProvider
      shaderProgramProvider{};
  static inline const WallVaoProvider vaoProvider{};
  static inline const TextureProvider textureProvider{
      std::string("textures/tile1.jpeg")};

  glm::mat4 model{1.0};
};
