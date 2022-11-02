module;

#include <array>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

export module floor;

import shader;

class FloorVaoProvider {
public:
  const GLuint& vao() const {
    if (glIsVertexArray(_vao) == GL_TRUE) return _vao;

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    std::array<glm::vec3, 24> vertices{{{-0.5, -0.5, 0.5},  {-0.5, 0.5, 0.5},
                                        {0.5, 0.5, 0.5},    {0.5, -0.5, 0.5},

                                        {-0.5, -0.5, -0.5}, {-0.5, 0.5, -0.5},
                                        {0.5, 0.5, -0.5},   {0.5, -0.5, -0.5},

                                        {0.5, 0.5, -0.5},   {-0.5, 0.5, -0.5},
                                        {-0.5, 0.5, 0.5},   {0.5, 0.5, 0.5},

                                        {-0.5, -0.5, 0.5},  {-0.5, -0.5, -0.5},
                                        {0.5, -0.5, -0.5},  {0.5, -0.5, 0.5},

                                        {0.5, 0.5, -0.5},   {0.5, -0.5, 0.5},
                                        {0.5, 0.5, -0.5},   {0.5, -0.5, 0.5},

                                        {-0.5, 0.5, -0.5},  {-0.5, -0.5, 0.5},
                                        {-0.5, 0.5, -0.5},  {-0.5, -0.5, 0.5}}};

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

export class FloorComponent {
public:
  FloorComponent(const glm::vec3& position) {
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3{0.2f, 0.01f, 0.2f});

    setUniformToProgram(shaderProgramProvider.program(), "color",
                        glm::vec4{0.8f});
  }

  void render(const glm::mat4& view, const glm::mat4& proj) const {
    glBindVertexArray(vaoProvider.vao());
    glUseProgram(shaderProgramProvider.program());
    setUniformToProgram(shaderProgramProvider.program(), "model", model);
    setUniformToProgram(shaderProgramProvider.program(), "view", view);
    setUniformToProgram(shaderProgramProvider.program(), "proj", proj);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (GLint i = 0; i <= 5; i++)
      glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
  }

private:
  static inline const BasicShaderProgramProvider shaderProgramProvider{};
  static inline const FloorVaoProvider vaoProvider{};

  glm::mat4 model{1.0};
};