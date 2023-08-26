#include "scene.h"

AnimatedSphereData generateRandomAnimatedSphereData() noexcept;

Scene::Scene(const float& viewAspectRatio) {
  updateViewAspectRatio(viewAspectRatio);
  addWalls();
  addFloor();
  addCeiling();
}

// void Scene::render(const glm::mat4& view, const SceneData& data,
//                    const glm::vec3& viewPosition,
//                    std::vector<std::shared_ptr<QuadTreeNode>> treeLeafs)
//                    const {
//   for (const auto& component : staticComponents)
//     std::visit([&](const auto& c) { c.render(view, proj); }, component);
//
//   for (const auto& component : lightingComponents)
//     std::visit(
//         [&](const auto& c) {
//           c.render(view, proj, viewPosition, lightSource.position());
//         },
//         component);
//
//   lightSource.render(view, proj);
//
//   renderSpheres(view, viewPosition, data.spheres);
//
//   if (!data.isBirdView)
//     for (const auto& cellingComponent : cellingComponents)
//       cellingComponent.render(view, proj, viewPosition,
//       lightSource.position());
//
//   if (data.isBirdView) {
//     for (auto& treeLeaf : treeLeafs) {
//       CameraComponent cameraComponent(
//           treeLeaf->firstPersonController->position());
//       cameraComponent.render(
//           view, proj,
//           treeLeaf->firstPersonController->horizontalAngleRadians(),
//           treeLeaf->firstPersonController->verticalAngleRadians(),
//           viewPosition, lightSource.position());
//     }
//   }
// }

void Scene::updateViewAspectRatio(const float& viewAspectRatio) {
  static float currentAspectRatio;
  if (currentAspectRatio != viewAspectRatio) [[unlikely]]
    proj = glm::mat4{
        glm::perspective(glm::radians(45.0f), viewAspectRatio, 0.01f, 100.0f)};
}

void Scene::addWalls() {
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

void Scene::addFloor() {
  for (size_t i = 0; i < 10; i++)
    for (size_t j = 0; j < 10; j++)
      lightingComponents.emplace_back(
          FloorComponent{glm::vec3{-0.9 + 0.2 * i, 0, -0.9 + 0.2 * j}});
}

void Scene::addCeiling() {
  for (size_t i = 0; i < 10; i++)
    for (size_t j = 0; j < 10; j++)
      cellingComponents.emplace_back(
          FloorComponent{glm::vec3{-0.9 + 0.2 * i, 1.2, -0.9 + 0.2 * j}});
}

void Scene::renderSpheres(const glm::mat4& view, const glm::vec3& viewPosition,
                          const std::vector<AnimatedSphereData>& data) const {
  for (size_t i = 0; i < data.size(); i++) {
    if (sphereComponents.size() <= i)
      sphereComponents.push_back(SphereComponent{});

    sphereComponents.at(i).render(view, proj, data.at(i).sphereData,
                                  viewPosition, lightSource.position());
  }
}

SceneController::SceneController() {
  const auto numSpheres{100};
  for (size_t i = 0; i < numSpheres; i++)
    data.spheres.push_back(generateRandomAnimatedSphereData());
}

void SceneController::updateSceneData(const bool& isBirdView,
                                      double currentTimestamp) {
  data.isBirdView = isBirdView;

  degreeCycleCounter =
      std::fmodf(static_cast<float>(currentTimestamp) * 50.0f, 360.0f);
  // degreeCycleCounter = std::fmodf(degreeCycleCounter + 0.5f, 360.0f);

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

const SceneData& SceneController::sceneData() const { return data; }

static AnimatedSphereData generateRandomAnimatedSphereData() noexcept {
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

  std::uniform_real_distribution<float> speedDistribution(10, 50);
  float speed{speedDistribution(engine)};

  return {.originalPosition{position},
          .movingScale{movingScale},
          .cycleOffset{cycleOffset},
          .sphereData{
              .position{position},
              .color{color},
          },
          .speed{speed}};
}
