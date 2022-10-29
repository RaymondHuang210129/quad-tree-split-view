module;

#include <array>
#include <variant>

#include <glm/gtc/matrix_transform.hpp>

export module scene;

import axes;
import grid;

using StaticComponent = std::variant<AxesComponent, GridComponent>;

export struct SceneData {};

export class Scene {
public:
  Scene(const float& viewAspectRatio) {
    updateViewAspectRatio(viewAspectRatio);
  }

  void render(const glm::mat4& view, const SceneData& data) const {
    for (const auto& component : staticComponents) {
      std::visit([&](const auto& c) { c.render(view, proj); }, component);
    }
  }

  void updateViewAspectRatio(const float& viewAspectRatio) {
    proj = glm::mat4{glm::perspective(glm::radians(45.0f), viewAspectRatio,
                                      0.0001f, 100.0f)};
  }

private:
  glm::mat4 proj{};
  const std::array<StaticComponent, 2> staticComponents{AxesComponent{},
                                                        GridComponent{}};
};
