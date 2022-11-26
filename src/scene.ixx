module;

#include <array>
#include <random>
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
import quad_tree;
import camera;
import user_control;

using StaticComponent =
    std::variant<AxesComponent, GridComponent, LightSourceComponent>;
using LighingComponent = std::variant<FloorComponent, WallComponent>;

struct AnimatedSphereData {
  glm::vec3 originalPosition;
  glm::vec3 movingScale;
  float cycleOffset;
  SphereData sphereData;
};

AnimatedSphereData generateRandomAnimatedSphereData() noexcept;

export struct SceneData {
  std::vector<AnimatedSphereData> spheres;
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
              const glm::vec3& viewPosition,
              std::vector<std::shared_ptr<QuadTreeNode>> treeLeafs) const {
    for (const auto& component : staticComponents)
      std::visit([&](const auto& c) { c.render(view, proj); }, component);

    for (const auto& component : lightingComponents)
      std::visit(
          [&](const auto& c) {
            c.render(view, proj, viewPosition, lightSource.position());
          },
          component);

    lightSource.render(view, proj);

    renderSpheres(view, viewPosition, data.spheres);

    if (!data.isBirdView)
      for (const auto& cellingComponent : cellingComponents)
        cellingComponent.render(view, proj, viewPosition,
                                lightSource.position());

    if (data.isBirdView) {
      for (auto& treeLeaf : treeLeafs) {
        CameraComponent cameraComponent(
            treeLeaf->firstPersonController->position());
        cameraComponent.render(
            view, proj,
            treeLeaf->firstPersonController->horizontalAngleRadians(),
            treeLeaf->firstPersonController->verticalAngleRadians(),
            viewPosition, lightSource.position());
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
  mutable std::vector<SphereComponent> sphereComponents{};
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

  void renderSpheres(const glm::mat4& view, const glm::vec3& viewPosition,
                     const std::vector<AnimatedSphereData>& data) const {
    for (size_t i = 0; i < data.size(); i++) {
      if (sphereComponents.size() <= i)
        sphereComponents.push_back(SphereComponent{});

      sphereComponents.at(i).render(view, proj, data.at(i).sphereData,
                                    viewPosition, lightSource.position());
    }
  }
};

export class SceneController {
public:
  SceneController() {
    const auto numSpheres{100};
    for (size_t i = 0; i < numSpheres; i++)
      data.spheres.push_back(generateRandomAnimatedSphereData());
  }

  void updateSceneData(const bool& isBirdView) {
    data.isBirdView = isBirdView;

    degreeCycleCounter = std::fmodf(degreeCycleCounter + 0.5f, 360.0f);

    for (auto& sphere : data.spheres) {
      sphere.sphereData.position.x =
          sphere.originalPosition.x +
          glm::sin(glm::radians(degreeCycleCounter + sphere.cycleOffset)) *
              sphere.movingScale.x;

      sphere.sphereData.position.z =
          sphere.originalPosition.z +
          glm::cos(glm::radians(degreeCycleCounter + sphere.cycleOffset)) *
              sphere.movingScale.z;
    }
  }

  const SceneData& sceneData() const { return data; }

private:
  SceneData data{.isBirdView{false}};

  float degreeCycleCounter{};
};

AnimatedSphereData generateRandomAnimatedSphereData() noexcept {
  std::random_device rd;
  std::mt19937 engine(rd());

  std::uniform_real_distribution<> positionDistribution(-0.75, 0.75);
  glm::vec3 position{
      positionDistribution(engine),
      0.05,
      positionDistribution(engine),
  };

  std::uniform_real_distribution<> movingScaleXDistribution(
      0.05, 0.9 - std::abs(position.x));
  std::uniform_real_distribution<> movingScaleYDistribution(
      0.05, 0.9 - std::abs(position.y));
  std::uniform_real_distribution<> movingScaleZDistribution(
      0.05, 0.9 - std::abs(position.z));
  glm::vec3 movingScale{
      movingScaleXDistribution(engine),
      0,
      movingScaleZDistribution(engine),
  };

  std::uniform_real_distribution<float> cycleOffsetDistribution(0, 360);
  float cycleOffset{cycleOffsetDistribution(engine)};

  std::uniform_real_distribution colorDistribution(0.4, 0.95);
  glm::vec3 color{
      colorDistribution(engine),
      colorDistribution(engine),
      colorDistribution(engine),
  };

  return {.originalPosition{position},
          .movingScale{movingScale},
          .cycleOffset{cycleOffset},
          .sphereData{
              .position{position},
              .color{color},
          }};
}
