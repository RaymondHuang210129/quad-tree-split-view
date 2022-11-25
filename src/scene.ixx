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
import light_source;

using StaticComponent =
    std::variant<AxesComponent, GridComponent, LightSourceComponent>;
using LighingComponent = std::variant<FloorComponent, WallComponent>;

export struct SceneData {
  std::array<SphereData, 3> spheres;
  bool isBirdView;
};

export class Scene {
public:
  Scene(const float& viewAspectRatio) {
    updateViewAspectRatio(viewAspectRatio);
    addWalls();
    addFloor();
    addCeiling();
  }

  void render(const glm::mat4& view, const SceneData& data,
              const glm::vec3& viewPosition) const {
    for (const auto& component : staticComponents)
      std::visit([&](const auto& c) { c.render(view, proj); }, component);

    for (const auto& component : lightingComponents)
      std::visit(
          [&](const auto& c) {
            c.render(view, proj, viewPosition, lightSource.position());
          },
          component);

    lightSource.render(view, proj);

    for (size_t i = 0; const auto& sphereComponent : sphereComponents) {
      sphereComponent.render(view, proj, data.spheres.at(i), viewPosition,
                             lightSource.position());
      i++;
    }

    if (!data.isBirdView) {
      for (const auto& cellingComponent : cellingComponents) {
        cellingComponent.render(view, proj, viewPosition,
                                lightSource.position());
      }
    }
  }

  void updateViewAspectRatio(const float& viewAspectRatio) {
    proj = glm::mat4{
        glm::perspective(glm::radians(45.0f), viewAspectRatio, 0.01f, 100.0f)};
  }

private:
  glm::mat4 proj{};
  std::vector<StaticComponent> staticComponents{AxesComponent{},
                                                GridComponent{}};
  std::vector<LighingComponent> lightingComponents{};
  std::vector<FloorComponent> cellingComponents{};
  std::vector<SphereComponent> sphereComponents{
      SphereComponent{glm::vec4{0.5, 0.3, 0.7, 1.0}},
      SphereComponent{glm::vec4{0.7, 0.3, 0.3, 1.0}},
      SphereComponent{glm::vec4{0.5, 0.7, 0.4, 1.0}},
  };
  LightSourceComponent lightSource{glm::vec3{0.3f, 0.99f, 0.8f}};

  void addWalls() {
    for (size_t i = 0; i < 3; i++)
      for (size_t j = 0; j < 10; j++) {
        lightingComponents.emplace_back(
            WallComponent{glm::vec3{-0.9 + 0.2 * j, 0.2 + i * 0.4, -1}, false});
        lightingComponents.emplace_back(
            WallComponent{glm::vec3{-0.9 + 0.2 * j, 0.2 + i * 0.4, 1}, false});
        lightingComponents.emplace_back(
            WallComponent{glm::vec3{-1, 0.2 + i * 0.4, -0.9 + 0.2 * j}, true});
        lightingComponents.emplace_back(
            WallComponent{glm::vec3{1, 0.2 + i * 0.4, -0.9 + 0.2 * j}, true});
      }
  }

  void addFloor() {
    for (size_t i = 0; i < 10; i++)
      for (size_t j = 0; j < 10; j++)
        lightingComponents.emplace_back(
            FloorComponent{glm::vec3{-0.9 + 0.2 * i, 0, -0.9 + 0.2 * j}});
  }

  void addCeiling() {
    for (size_t i = 0; i < 10; i++)
      for (size_t j = 0; j < 10; j++)
        cellingComponents.emplace_back(
            FloorComponent{glm::vec3{-0.9 + 0.2 * i, 1.2, -0.9 + 0.2 * j}});
  }
};

export class SceneController {
public:
  void updateSceneData(const bool& isBirdView) {
    data.isBirdView = isBirdView;

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
                 }},
                 .isBirdView{false}};

  float degreeCycleCounter{};
};
