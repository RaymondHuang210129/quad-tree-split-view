module;

#include <glm/gtc/type_ptr.hpp>

export module scene;

import axes;

export struct SceneData {};

export class Scene {
public:
  Scene(const float& viewAspectRatio) {
    updateViewAspectRatio(viewAspectRatio);
  }

  void render(const glm::mat4& view, const SceneData& data) const {
    axesComponent.render(view, proj);
  }

  void updateViewAspectRatio(const float& viewAspectRatio) {
    proj = glm::mat4{glm::perspective(glm::radians(45.0f), viewAspectRatio,
                                      0.0001f, 100.0f)};
  }

private:
  glm::mat4 proj{};

  const AxesComponent axesComponent{};
};
