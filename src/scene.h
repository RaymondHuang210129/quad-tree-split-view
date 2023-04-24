#pragma once
#include <array>
#include <random>
#include <variant>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "axes.h"
#include "camera.h"
#include "floor.h"
#include "grid.h"
#include "light_source.h"
#include "quad_tree.h"
#include "sphere.h"
#include "user_control.h"
#include "wall.h"

using StaticComponent =
    std::variant<AxesComponent, GridComponent, LightSourceComponent>;
using LighingComponent = std::variant<FloorComponent, WallComponent>;

struct AnimatedSphereData {
  glm::vec3 originalPosition;
  glm::vec3 movingScale;
  float cycleOffset;
  SphereData sphereData;
  float speed;
};

struct SceneData {
  std::vector<AnimatedSphereData> spheres;
  bool isBirdView;
};

class Scene {
public:
  Scene(const float& viewAspectRatio);
  void render(const glm::mat4& view, const SceneData& data,
              const glm::vec3& viewPosition,
              std::vector<std::shared_ptr<QuadTreeNode>> treeLeafs) const;
  void updateViewAspectRatio(const float& viewAspectRatio);

private:
  glm::mat4 proj{};
  std::vector<StaticComponent> staticComponents{AxesComponent{},
                                                GridComponent{}};
  std::vector<LighingComponent> lightingComponents{};
  std::vector<FloorComponent> cellingComponents{};
  mutable std::vector<SphereComponent> sphereComponents{};
  LightSourceComponent lightSource{glm::vec3{0.3f, 0.99f, 0.8f}};

  void addWalls();
  void addFloor();
  void addCeiling();
  void renderSpheres(const glm::mat4& view, const glm::vec3& viewPosition,
                     const std::vector<AnimatedSphereData>& data) const;
};

class SceneController {
public:
  SceneController();
  void updateSceneData(const bool& isBirdView, double currentTimestamp);
  const SceneData& sceneData() const;

private:
  SceneData data{.isBirdView{false}};

  float degreeCycleCounter{};
};
