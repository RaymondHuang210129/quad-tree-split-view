module;

#define _USE_MATH_DEFINES

#include <array>
#include <cmath>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

export module sphere;

import shader;

const std::vector<glm::vec3> tessellateIcosahedron(const size_t& divisionCount);
const std::vector<glm::vec3>
subdivideTriangle(const std::array<glm::vec3, 3>& triangle, const size_t& step);

export class SphereVaoProvider {
public:
  const std::vector<glm::vec3> vertices{tessellateIcosahedron(3)};

  const GLuint& vao() const {
    if (glIsVertexArray(_vao) == GL_TRUE) return _vao;

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void*>(0));

    glEnableVertexAttribArray(0);
    return _vao;
  }

private:
  mutable GLuint _vao{};
};

export struct SphereData {
  glm::vec3 position;
  glm::vec3 color;
};

export class SphereComponent {
public:
  SphereComponent() {
    setUniformToProgram(shaderProgramProvider.program(), "lightAmbient",
                        glm::vec3{0.2f});
    setUniformToProgram(shaderProgramProvider.program(), "lightDiffuse",
                        glm::vec3{1.0f});
    setUniformToProgram(shaderProgramProvider.program(), "lightSpecular",
                        glm::vec3{1.0f});
    setUniformToProgram(shaderProgramProvider.program(), "luminousIntensity",
                        GLfloat(1));
  }

  void render(const glm::mat4& view, const glm::mat4& proj,
              const SphereData& data, const glm::vec3& viewPosition,
              const glm::vec3& lightPosition) const {
    glBindVertexArray(vaoProvider.vao());
    glUseProgram(shaderProgramProvider.program());

    glm::mat4 model{1.0f};
    model = glm::translate(model, data.position);
    model = glm::scale(model, glm::vec3{0.05f});

    setUniformToProgram(shaderProgramProvider.program(), "model", model);
    setUniformToProgram(shaderProgramProvider.program(), "view", view);
    setUniformToProgram(shaderProgramProvider.program(), "proj", proj);
    setUniformToProgram(shaderProgramProvider.program(), "color",
                        glm::vec4{data.color, 1.0f});
    setUniformToProgram(shaderProgramProvider.program(), "viewPosition",
                        viewPosition);
    setUniformToProgram(shaderProgramProvider.program(), "lightPosition",
                        lightPosition);

    glDrawArrays(GL_TRIANGLES, 0,
                 static_cast<GLsizei>(vaoProvider.vertices.size()));
  }

private:
  static inline const SphereLightingShaderProgramProvider
      shaderProgramProvider{};
  static inline const SphereVaoProvider vaoProvider{};
};

const std::vector<glm::vec3>
tessellateIcosahedron(const size_t& divisionCount) {
  const std::array<glm::vec3, 5> baseVertices{
      glm::vec3{std::cos(0), std::sin(0), 0},
      {std::cos(2.f / 3.f * M_PI), std::sin(2.f / 3.f * M_PI), 0},
      {std::cos(4.f / 3.f * M_PI), std::sin(4.f / 3.f * M_PI), 0},
      {0, 0, 1},
      {0, 0, -1}};

  const std::array<std::array<glm::vec3, 3>, 6> triangles{{
      {baseVertices.at(0), baseVertices.at(1), baseVertices.at(3)},
      {baseVertices.at(0), baseVertices.at(2), baseVertices.at(3)},
      {baseVertices.at(1), baseVertices.at(2), baseVertices.at(3)},
      {baseVertices.at(0), baseVertices.at(1), baseVertices.at(4)},
      {baseVertices.at(0), baseVertices.at(2), baseVertices.at(4)},
      {baseVertices.at(1), baseVertices.at(2), baseVertices.at(4)},
  }};

  std::vector<glm::vec3> vertices{};

  for (const auto& triangle : triangles) {
    const auto divided{subdivideTriangle(triangle, divisionCount)};
    vertices.insert(vertices.end(), divided.begin(), divided.end());
  }

  return vertices;
}

const std::vector<glm::vec3>
subdivideTriangle(const std::array<glm::vec3, 3>& triangle,
                  const size_t& step) {
  if (step == 0) return {triangle.begin(), triangle.end()};

  const glm::vec3 v01{glm::normalize((triangle.at(0) + triangle.at(1)) / 2.f)};
  const glm::vec3 v12{glm::normalize((triangle.at(1) + triangle.at(2)) / 2.f)};
  const glm::vec3 v20{glm::normalize((triangle.at(2) + triangle.at(0)) / 2.f)};

  std::vector<glm::vec3> vertices{};
  const std::array<glm::vec3, 3> topTriangle{triangle.at(0), v01, v20};
  const auto v1{subdivideTriangle(topTriangle, step - 1)};
  vertices.insert(vertices.end(), v1.begin(), v1.end());

  const std::array<glm::vec3, 3> leftTriangle{v01, triangle.at(1), v12};
  const auto v2{subdivideTriangle(leftTriangle, step - 1)};
  vertices.insert(vertices.end(), v2.begin(), v2.end());

  const std::array<glm::vec3, 3> rightTriangle{v20, v12, triangle.at(2)};
  const auto v3{subdivideTriangle(rightTriangle, step - 1)};
  vertices.insert(vertices.end(), v3.begin(), v3.end());

  const std::array<glm::vec3, 3> midTriangle{v01, v12, v20};
  const auto v4{subdivideTriangle(midTriangle, step - 1)};
  vertices.insert(vertices.end(), v4.begin(), v4.end());

  return vertices;
}
