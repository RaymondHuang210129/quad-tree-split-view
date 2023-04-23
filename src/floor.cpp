#include "floor.h"

const GLuint& FloorVaoProvider::vao() const {
  if (glIsVertexArray(_vao) == GL_TRUE) return _vao;

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  GLuint vbo{};
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  typedef struct {
    glm::vec3 position;
    glm::vec2 textureCoordinate;
    glm::vec3 normal;
  } vertexAttributes;

  std::array<vertexAttributes, 24> vertices{
      {{{-0.5, -0.5, 0.5}, {0.0, 0.0}, {0.0, 0.0, 1.0}},
       {{-0.5, 0.5, 0.5}, {0.0, 1.0}, {0.0, 0.0, 1.0}},
       {{0.5, 0.5, 0.5}, {1.0, 1.0}, {0.0, 0.0, 1.0}},
       {{0.5, -0.5, 0.5}, {1.0, 0.0}, {0.0, 0.0, 1.0}}, // upper z plane

       {{-0.5, -0.5, -0.5}, {0.0, 0.0}, {0.0, 0.0, -1.0}},
       {{-0.5, 0.5, -0.5}, {0.0, 1.0}, {0.0, 0.0, -1.0}},
       {{0.5, 0.5, -0.5}, {1.0, 1.0}, {0.0, 0.0, -1.0}},
       {{0.5, -0.5, -0.5}, {1.0, 0.0}, {0.0, 0.0, -1.0}}, // lower z plane

       {{0.5, 0.5, -0.5}, {0.0, 0.0}, {0.0, 1.0, 0.0}},
       {{-0.5, 0.5, -0.5}, {0.0, 1.0}, {0.0, 1.0, 0.0}},
       {{-0.5, 0.5, 0.5}, {1.0, 1.0}, {0.0, 1.0, 0.0}},
       {{0.5, 0.5, 0.5}, {1.0, 0.0}, {0.0, 1.0, 0.0}}, // right y plane

       {{-0.5, -0.5, 0.5}, {0.0, 0.0}, {0.0, -1.0, 0.0}},
       {{-0.5, -0.5, -0.5}, {0.0, 1.0}, {0.0, -1.0, 0.0}},
       {{0.5, -0.5, -0.5}, {1.0, 1.0}, {0.0, -1.0, 0.0}},
       {{0.5, -0.5, 0.5}, {1.0, 0.0}, {0.0, -1.0, 0.0}}, // left y plane

       {{0.5, 0.5, -0.5}, {0.0, 0.0}, {1.0, 0.0, 0.0}},
       {{0.5, -0.5, 0.5}, {0.0, 1.0}, {1.0, 0.0, 0.0}},
       {{0.5, 0.5, -0.5}, {1.0, 1.0}, {1.0, 0.0, 0.0}},
       {{0.5, -0.5, 0.5}, {1.0, 0.0}, {1.0, 0.0, 0.0}}, // front x plane

       {{-0.5, 0.5, -0.5}, {0.0, 0.0}, {-1.0, 0.0, 0.0}},
       {{-0.5, -0.5, 0.5}, {0.0, 1.0}, {-1.0, 0.0, 0.0}},
       {{-0.5, 0.5, -0.5}, {1.0, 1.0}, {-1.0, 0.0, 0.0}},
       {{-0.5, -0.5, 0.5}, {1.0, 0.0}, {-1.0, 0.0, 0.0}}}}; // back x plane

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertexAttributes),
               vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, sizeof(vertexAttributes::position) / sizeof(GLfloat),
                        GL_FLOAT, GL_FALSE, sizeof(vertexAttributes),
                        (GLvoid*)offsetof(vertexAttributes, position));
  glVertexAttribPointer(
      1, sizeof(vertexAttributes::textureCoordinate) / sizeof(GLfloat),
      GL_FLOAT, GL_FALSE, sizeof(vertexAttributes),
      (GLvoid*)offsetof(vertexAttributes, textureCoordinate));
  glVertexAttribPointer(2, sizeof(vertexAttributes::normal) / sizeof(GLfloat),
                        GL_FLOAT, GL_FALSE, sizeof(vertexAttributes),
                        (GLvoid*)offsetof(vertexAttributes, normal));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  return _vao;
}

FloorComponent::FloorComponent(const glm::vec3& position) {
  model = glm::translate(model, position);
  model = glm::scale(model, glm::vec3{0.2f, 0.01f, 0.2f});

  setUniformToProgram(shaderProgramProvider.program(), "lightAmbient",
                      glm::vec3{0.2f});
  setUniformToProgram(shaderProgramProvider.program(), "lightDiffuse",
                      glm::vec3{1.0f});
  setUniformToProgram(shaderProgramProvider.program(), "lightSpecular",
                      glm::vec3{1.0f});
  setUniformToProgram(shaderProgramProvider.program(), "luminousIntensity",
                      GLfloat(1));
};

void FloorComponent::render(const glm::mat4& view, const glm::mat4& proj,
                            const glm::vec3& viewPosition,
                            const glm::vec3& lightPosition) const {
  glBindVertexArray(vaoProvider.vao());
  glBindTexture(GL_TEXTURE_2D, textureProvider.texture());
  glUseProgram(shaderProgramProvider.program());
  setUniformToProgram(shaderProgramProvider.program(), "model", model);
  setUniformToProgram(shaderProgramProvider.program(), "view", view);
  setUniformToProgram(shaderProgramProvider.program(), "proj", proj);
  setUniformToProgram(shaderProgramProvider.program(), "viewPosition",
                      viewPosition);
  setUniformToProgram(shaderProgramProvider.program(), "lightPosition",
                      lightPosition);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  for (GLint i = 0; i <= 5; i++)
    glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
};
