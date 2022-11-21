module;

#define _USE_MATH_DEFINES

#include <array>
#include <cmath>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

export module light_source;

import shader;
import sphere;

export class LightSourceComponent {
public:
  LightSourceComponent(const glm::vec3& position) {
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3{0.01f});
    _position = position;
  }

  void render(const glm::mat4& view, const glm::mat4& proj) const {
    glBindVertexArray(vaoProvider.vao());
    glUseProgram(shaderProgramProvider.program());

    setUniformToProgram(shaderProgramProvider.program(), "model", model);
    setUniformToProgram(shaderProgramProvider.program(), "view", view);
    setUniformToProgram(shaderProgramProvider.program(), "proj", proj);
    setUniformToProgram(shaderProgramProvider.program(), "color", _color);

    glDrawArrays(GL_TRIANGLES, 0,
                 static_cast<GLsizei>(vaoProvider.vertices.size()));
  }

  const glm::vec3& position() const { return _position; }

private:
  static inline const BasicShaderProgramProvider shaderProgramProvider{};
  static inline const SphereVaoProvider vaoProvider{};

  glm::vec3 _position{0.0};
  glm::mat4 model{1.0};
  const glm::vec4 _color{1.0f};
};
