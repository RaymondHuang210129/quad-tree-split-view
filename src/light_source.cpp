#include "light_source.h"

const GLuint& LightSourceVaoProvider::vao() const {
  static auto _ = std::invoke([this] {
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void*>(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void*>(0));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    return 0;
  });
  return _vao;
}

LightSourceComponent::LightSourceComponent(const glm::vec3& position) {
  model = glm::translate(model, position);
  model = glm::scale(model, glm::vec3{0.01f});
  _position = position;
};

void LightSourceComponent::render(const glm::mat4& view,
                                  const glm::mat4& proj) const {
  glBindVertexArray(vaoProvider.vao());
  glUseProgram(shaderProgramProvider.program());

  setUniformToProgram(shaderProgramProvider.program(), "model", model);
  setUniformToProgram(shaderProgramProvider.program(), "view", view);
  setUniformToProgram(shaderProgramProvider.program(), "proj", proj);
  setUniformToProgram(shaderProgramProvider.program(), "color", _color);

  glDrawArrays(GL_TRIANGLES, 0,
               static_cast<GLsizei>(vaoProvider.vertices.size()));
};

const glm::vec3& LightSourceComponent::position() const { return _position; };
