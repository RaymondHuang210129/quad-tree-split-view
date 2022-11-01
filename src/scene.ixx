module;

#include <array>
#include <variant>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

export module scene;

import axes;
import grid;
import wall;
import floor;

using StaticComponent =
    std::variant<AxesComponent, GridComponent, WallComponent, FloorComponent>;

export struct SceneData {};

export class Scene {
public:
  Scene(const float& viewAspectRatio) {
    updateViewAspectRatio(viewAspectRatio);
    addWalls();
    addFloor();
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
  std::vector<StaticComponent> staticComponents{AxesComponent{},
                                                GridComponent{}};

  void addWalls() {
    for (size_t i = 0; i < 3; i++) {
      for (size_t j = 0; j < 10; j++) {
        staticComponents.emplace_back(
            WallComponent{glm::vec3{-0.9 + 0.2 * j, 0.2 + i * 0.4, -1}, false});
        staticComponents.emplace_back(
            WallComponent{glm::vec3{-0.9 + 0.2 * j, 0.2 + i * 0.4, 1}, false});
        staticComponents.emplace_back(
            WallComponent{glm::vec3{-1, 0.2 + i * 0.4, -0.9 + 0.2 * j}, true});
        staticComponents.emplace_back(
            WallComponent{glm::vec3{1, 0.2 + i * 0.4, -0.9 + 0.2 * j}, true});
      }

      for (size_t j = 0; j < 5; j++) {
        staticComponents.emplace_back(
            WallComponent{glm::vec3{-0.9 + 0.2 * j, 0.2 + i * 0.4, 0}, false});
      }
    }
  }

  void addFloor() {
    for (size_t i = 0; i < 10; i++) {
      for (size_t j = 0; j < 10; j++) {
        staticComponents.emplace_back(
            FloorComponent{glm::vec3{-0.9 + 0.2 * i, 0, -0.9 + 0.2 * j}});

        staticComponents.emplace_back(
            FloorComponent{glm::vec3{-0.9 + 0.2 * i, 1.2, -0.9 + 0.2 * j}});
      }
    }
  }
};
