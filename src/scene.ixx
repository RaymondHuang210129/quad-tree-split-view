module;

#include <glm/gtc/type_ptr.hpp>

export module scene;

import axes;

export struct SceneData {};

export class Scene {
public:
  void render(const float& viewAspectRatio, const SceneData& data) const {
    const glm::mat4 view{glm::lookAt(glm::vec3{2, 2, 1}, glm::vec3{0, 0, 0},
                                     glm::vec3{0, 1, 0})};

    const glm::mat4 proj{glm::perspective(glm::radians(45.0f), viewAspectRatio,
                                          0.0001f, 100.0f)};

    axesComponent.render(view, proj);
  }

private:
  const AxesComponent axesComponent{};
};