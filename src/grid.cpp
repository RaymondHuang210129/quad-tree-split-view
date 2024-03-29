#include "grid.h"

const GLuint& GridVaoProvider::vao() const {
  static auto _ = std::invoke([this] {
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    constexpr auto gridMin{-0.5f};
    constexpr auto gridMax{0.5f};
    const auto divisionSize{std::abs(gridMax - gridMin) / gridCount};

    std::vector<glm::vec3> vertices{};

    for (size_t i = 0; i <= gridCount; i++) {
      vertices.emplace_back(
          glm::vec3{gridMin + divisionSize * i, gridMin, 0.0f});
      vertices.emplace_back(
          glm::vec3{gridMin + divisionSize * i, gridMax, 0.0f});
    }
    for (size_t i = 0; i <= gridCount; i++) {
      vertices.push_back({gridMin, gridMin + divisionSize * i, 0.0f});
      vertices.push_back({gridMax, gridMin + divisionSize * i, 0.0f});
    }

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void*>(0));

    glEnableVertexAttribArray(0);
    return 0;
  });
  return _vao;
};

GridComponent::GridComponent() {
  auto model{glm::mat4(1.0)};
  model = glm::scale(model, glm::vec3{2.0});
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3{1.0, 0.0, 0.0});

  setUniformToProgram(shaderProgramProvider.program(), "model", model);
  setUniformToProgram(shaderProgramProvider.program(), "color",
                      glm::vec4{0.6f});
};

void GridComponent::render(const glm::mat4& view, const glm::mat4& proj) const {
  glBindVertexArray(vaoProvider.vao());
  glUseProgram(shaderProgramProvider.program());
  setUniformToProgram(shaderProgramProvider.program(), "view", view);
  setUniformToProgram(shaderProgramProvider.program(), "proj", proj);

  glDrawArrays(GL_LINES, 0, (vaoProvider.gridCount + 1) * 2);
  glDrawArrays(GL_LINES, (vaoProvider.gridCount + 1) * 2,
               (vaoProvider.gridCount + 1) * 2);
};
