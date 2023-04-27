#include "camera.h"

UserControlData CameraComponent::userData;

const GLuint& CameraVaoProvider::vao() const {
  static auto _ = std::invoke([this] {
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    GLuint vbo{};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    typedef struct {
      glm::vec3 position;
      glm::vec3 normal;
    } vertexAttributes;

    std::array<vertexAttributes, numSurfaces * numPointsOfSurface> vertices{{
        {{-1, -1, 1}, {0, 0, 1}},
        {{-1, 1, 1}, {0, 0, 1}},
        {{1, 1, 1}, {0, 0, 1}},
        {{1, -1, 1}, {0, 0, 1}}, // upper z plane

        {{-1, -1, -1}, {0, 0, -1}},
        {{-1, 1, -1}, {0, 0, -1}},
        {{1, 1, -1}, {0, 0, -1}},
        {{1, -1, -1}, {0, 0, -1}}, // lower z plane

        {{1, 1, -1}, {0, 1, 0}},
        {{-1, 1, -1}, {0, 1, 0}},
        {{-1, 1, 1}, {0, 1, 0}},
        {{1, 1, 1}, {0, 1, 0}}, // right y plane

        {{-1, -1, 1}, {0, -1, 0}},
        {{-1, -1, -1}, {0, -1, 0}},
        {{1, -1, -1}, {0, -1, 0}},
        {{1, -1, 1}, {0, -1, 0}}, // left y plane

        {{1, -1, -1}, {1, 0, 0}},
        {{1, -1, 1}, {1, 0, 0}},
        {{1, 1, 1}, {1, 0, 0}},
        {{1, 1, -1}, {1, 0, 0}}, // front x plane

        {{-1, -1, -1}, {-1, 0, 0}},
        {{-1, -1, 1}, {-1, 0, 0}},
        {{-1, 1, 1}, {-1, 0, 0}},
        {{-1, 1, -1}, {-1, 0, 0}}, // back x plane

        {{0.5, 0.5, 1}, {0, 1, 0}},
        {{-0.5, 0.5, 1}, {0, 1, 0}},
        {{-1, 1, 3}, {0, 1, 0}},
        {{1, 1, 3}, {0, 1, 0}}, // right y lens hood

        {{-1, -1, 3}, {0, -1, 0}},
        {{-0.5, -0.5, 1}, {0, -1, 0}},
        {{0.5, -0.5, 1}, {0, -1, 0}},
        {{1, -1, 3}, {0, -1, 0}}, // left y lens hood

        {{0.5, -0.5, 1}, {1, 0, 0}},
        {{1, -1, 3}, {1, 0, 0}},
        {{1, 1, 3}, {1, 0, 0}},
        {{0.5, 0.5, 1}, {1, 0, 0}}, // front x lens hood

        {{-0.5, -0.5, 1}, {-1, 0, 0}},
        {{-1, -1, 3}, {-1, 0, 0}},
        {{-1, 1, 3}, {-1, 0, 0}},
        {{-0.5, 0.5, 1}, {-1, 0, 0}}, // back x lens hood

    }};

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertexAttributes),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,
                          sizeof(vertexAttributes::position) / sizeof(GLfloat),
                          GL_FLOAT, GL_FALSE, sizeof(vertexAttributes),
                          (GLvoid*)offsetof(vertexAttributes, position));
    glVertexAttribPointer(1,
                          sizeof(vertexAttributes::position) / sizeof(GLfloat),
                          GL_FLOAT, GL_FALSE, sizeof(vertexAttributes),
                          (GLvoid*)offsetof(vertexAttributes, normal));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    return 0;
  });
  return _vao;
}

void CameraComponent::updateUserControlData(const UserControlData& userData) {
  CameraComponent::userData = userData;
}

CameraComponent::CameraComponent(const glm::vec3& position) {
  model = glm::translate(model, position);
  model = glm::scale(model, glm::vec3{0.05f, 0.05f, 0.05f});
  setUniformToProgram(shaderProgramProvider.program(), "lightAmbient",
                      glm::vec3{0.2f});
  setUniformToProgram(shaderProgramProvider.program(), "lightDiffuse",
                      glm::vec3{1.0f});
  setUniformToProgram(shaderProgramProvider.program(), "lightSpecular",
                      glm::vec3{1.0f});
  setUniformToProgram(shaderProgramProvider.program(), "luminousIntensity",
                      GLfloat(1));
}

void CameraComponent::render(const glm::mat4& view, const glm::mat4& proj,
                             double horizontalAngleRadians,
                             double verticalAngleRadians,
                             const glm::vec3& viewPosition,
                             const glm::vec3& lightPosition) const {
  glBindVertexArray(vaoProvider.vao());
  glUseProgram(shaderProgramProvider.program());
  model = glm::rotate(model, static_cast<float>(horizontalAngleRadians),
                      glm::vec3{0.0f, 1.0f, 0.0f});
  model = glm::rotate(model, static_cast<float>(verticalAngleRadians),
                      glm::vec3{-1.0f, 0.0f, 0.0f});
  setUniformToProgram(shaderProgramProvider.program(), "color",
                      glm::vec4{0.2, 0.2, 0.2, 1.0});
  setUniformToProgram(shaderProgramProvider.program(), "model", model);
  setUniformToProgram(shaderProgramProvider.program(), "view", view);
  setUniformToProgram(shaderProgramProvider.program(), "proj", proj);
  setUniformToProgram(shaderProgramProvider.program(), "viewPosition",
                      viewPosition);
  setUniformToProgram(shaderProgramProvider.program(), "lightPosition",
                      lightPosition);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  for (GLint i = 0; i < numSurfaces; i++)
    glDrawArrays(GL_TRIANGLE_FAN, i * numPointsOfSurface, numPointsOfSurface);
};
