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
import sphere;

using StaticComponent =
    std::variant<AxesComponent, GridComponent, WallComponent, FloorComponent>;

export struct SceneData {
  std::array<SphereData, 3> spheres;
};

export class Scene {
public:
  Scene(const float& viewAspectRatio) {
    updateViewAspectRatio(viewAspectRatio);
    addWalls();
    addFloor();
  }

  void render(const glm::mat4& view, const SceneData& data) const {
    for (const auto& component : staticComponents)
      std::visit([&](const auto& c) { c.render(view, proj); }, component);

    for (size_t i = 0; const auto& sphereComponent : sphereComponents) {
      sphereComponent.render(view, proj, data.spheres.at(i));
      i++;
    }
  }

  void updateViewAspectRatio(const float& viewAspectRatio) {
    proj = glm::mat4{glm::perspective(glm::radians(45.0f), viewAspectRatio,
                                      0.0001f, 100.0f)};
  }

private:
  glm::mat4 proj{};
  std::vector<StaticComponent> staticComponents{
      AxesComponent{},
      GridComponent{},
  };
  std::vector<SphereComponent> sphereComponents{
      SphereComponent{glm::vec4{0.5, 0.3, 0.7, 1.0}},
      SphereComponent{glm::vec4{0.7, 0.3, 0.3, 1.0}},
      SphereComponent{glm::vec4{0.5, 0.7, 0.4, 1.0}},
  };

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

export class SceneController {
public:
  void updateSceneData() {
    degreeCycleCounter = std::fmodf(degreeCycleCounter + 1, 360.0f);

    data.spheres.at(0).position.x =
        data.spheres.at(0).position.x +
        std::sin(glm::radians(degreeCycleCounter)) / 200;

    data.spheres.at(1).position.z =
        data.spheres.at(1).position.z +
        std::cos(glm::radians(degreeCycleCounter)) / 160;

    data.spheres.at(2).position.x =
        data.spheres.at(2).position.x +
        std::cos(glm::radians(degreeCycleCounter) + 1.2f) / 160;
  }

  const SceneData& sceneData() const { return data; }

private:
  SceneData data{.spheres{{
      SphereData{{-0.5, 0.05, -0.5}},
      SphereData{{0.5, 0.05, 0.2}},
      SphereData{{0.2, 0.05, 0.5}},
  }}};

  float degreeCycleCounter{};
};
