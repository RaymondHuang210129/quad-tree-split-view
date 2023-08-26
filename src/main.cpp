#include <algorithm>
#include <array>
#include <iostream>
#include <random>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>

#include "fps_counter.h"
#include "global_timer.h"
#include "quad_tree.h"
#include "raii_glfw.h"
#include "scene.h"
#include "user_control.h"

#include "axes.h"
#include "camera.h"
#include "ceiling.h"
#include "floor.h"
#include "sphere.h"
#include "wall.h"

class VoidComponent {
public:
  void render() const { std::cout << "Component void render()" << std::endl; }
  static void updateUserControlData(const UserControlData& userData) {
    std::cout << "Component void user updated" << std::endl;
  }
  void updatePosition(const double& currentTimestamp) {
    std::cout << "Component void position updated" << std::endl;
  }
};

class IntComponent {
public:
  void render(const int& i) const {
    std::cout << "Component int render()" << std::endl;
  }
  static void updateUserControlData(const UserControlData& userData) {
    std::cout << "Component int user updated" << std::endl;
  }
};

class MatMatComponent {
public:
  void render(const glm::mat4& i, const glm::mat4& j) const {
    std::cout << "Component mat mat render()" << std::endl;
  }
  void updatePosition(const double& currentTimestamp) {
    std::cout << "Component mat mat position updated" << std::endl;
  }
};

class MatMatVecVecComponent {
public:
  void render(const glm::mat4& i, const glm::mat4& j, const glm::vec3& k,
              const glm::vec3& l) const {
    std::cout << "Component mat mat vec vec render()" << std::endl;
  }
};

const float viewAspectRatio(const int& width, const int& height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) noexcept;
std::vector<WallComponent> generateWall();
std::vector<FloorComponent> generateFloor();
std::vector<CeilingComponent> generateCeiling();
std::vector<SphereComponent> generateSpheres();
std::vector<CameraComponent> generateDefaultCamera(const glm::vec3& position);

static const glm::vec3 generateRandomPosition() noexcept;

struct WindowUserData {
  std::vector<std::shared_ptr<QuadTreeNode>> quadTree;
  std::vector<CameraComponent>&
      cameras; // note: reference to CameraComponents in Scene object
  bool isBirdView;
};

int main() {
  const RaiiGlfw raiiGlfw{};

  constexpr int defaultWidth{720};
  constexpr int defaultHeight{720};
  constexpr char windowTitle[] = "Quad Tree Split Views";

  GLFWwindow* window = glfwCreateWindow(defaultWidth, defaultHeight,
                                        windowTitle, nullptr, nullptr);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  SceneController sceneController{}; // deprecate

  glm::vec3 defaultViewPosition{0.0f, 0.2f, 0.8f};

  Scene scene{viewAspectRatio(defaultWidth, defaultHeight)};
  SceneV2<WallComponent, FloorComponent, CeilingComponent, SphereComponent,
          CameraComponent>
      scene2{viewAspectRatio(defaultWidth, defaultHeight),
             generateWall(),
             generateFloor(),
             generateCeiling(),
             generateSpheres(),
             generateDefaultCamera(defaultViewPosition)};

  WindowUserData windowUserData{.quadTree{std::make_shared<QuadTreeNode>(
                                    1.0f, 1.0f, 0.0f, 0.0f,
                                    std::make_shared<FirstPersonController>(
                                        window, defaultViewPosition))},
                                .cameras{scene2.getVector<CameraComponent>()},
                                .isBirdView{false}};

  glfwSetWindowUserPointer(window, &windowUserData);

  glfwSetKeyCallback(window, keyCallback);

  GlobalTimer globalTimer{};

  FpsCounter fpsCounter{window, windowTitle, globalTimer.getCurrentTime()};

  FirstPersonController firstPersonController{window, glm::vec3{}};

  while (!glfwWindowShouldClose(window)) {

    globalTimer.updateTime();

    fpsCounter.updateFramerate(globalTimer.getCurrentTime());

    firstPersonController.updateView();

    // sceneController.updateSceneData(windowUserData.isBirdView,
    //                                 globalTimer.getCurrentTime());

    scene2.updatePosition(globalTimer.getCurrentTime());
    scene2.updateUserControlData(firstPersonController.getUserControlData());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int windowWidth{}, windowHeight{};
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    if (windowUserData.isBirdView) {
      glViewport(0, 0, windowWidth, windowHeight);
      // scene.updateViewAspectRatio(viewAspectRatio(windowWidth,
      // windowHeight)); scene.render(glm::lookAt({1.25, 4, 1.25}, glm::vec3{0},
      // {0, 1, 0}),
      //              sceneController.sceneData(), glm::vec3{1.5, 1.5, 1.5},
      //              getQuadTreeLeaves(windowUserData.quadTree));
      scene2.updateViewAspectRatio(viewAspectRatio(windowWidth, windowHeight));
      scene2.render<WallComponent, FloorComponent, SphereComponent,
                    CameraComponent>(
          glm::lookAt({1.25, 4, 1.25}, glm::vec3{0}, {0, 1, 0}),
          sceneController.sceneData(), glm::vec3{1.5, 1.5, 1.5},
          getQuadTreeLeaves(windowUserData.quadTree));
    } else {
      for (auto& leaf : getQuadTreeLeaves(windowUserData.quadTree)) {
        leaf->firstPersonController->updateView();

        glViewport(static_cast<GLint>(leaf->x * windowWidth),
                   static_cast<GLint>(leaf->y * windowHeight),
                   static_cast<GLsizei>(leaf->width * windowWidth),
                   static_cast<GLsizei>(leaf->height * windowHeight));
        // scene.updateViewAspectRatio(
        //     viewAspectRatio(static_cast<int>(leaf->width * windowWidth),
        //                     static_cast<int>(leaf->height * windowHeight)));
        // scene.render(leaf->firstPersonController->view(),
        //              sceneController.sceneData(),
        //              leaf->firstPersonController->position(),
        //              getQuadTreeLeaves(windowUserData.quadTree));
        scene2.updateViewAspectRatio(
            viewAspectRatio(static_cast<int>(leaf->width * windowWidth),
                            static_cast<int>(leaf->height * windowHeight)));
        scene2.render<WallComponent, FloorComponent, CeilingComponent,
                      SphereComponent>(
            leaf->firstPersonController->view(), sceneController.sceneData(),
            leaf->firstPersonController->position(),
            getQuadTreeLeaves(windowUserData.quadTree));
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  /* SceneV2 test */

  SceneV2<VoidComponent, MatMatComponent, MatMatVecVecComponent, IntComponent>
      scenev2Test{
          1.0f, std::vector<VoidComponent>{VoidComponent{}},
          std::vector<MatMatComponent>{MatMatComponent{}, MatMatComponent{}},
          std::vector<MatMatVecVecComponent>{MatMatVecVecComponent{},
                                             MatMatVecVecComponent{},
                                             MatMatVecVecComponent{}},
          std::vector<IntComponent>{IntComponent{}}};

  scenev2Test.addComponent(VoidComponent{});
  const auto& c2Vector = scenev2Test.getVector<VoidComponent>();
  scenev2Test.renderAll(glm::mat4{}, SceneData{}, glm::vec3{}, {});
  scenev2Test.render<MatMatVecVecComponent, VoidComponent>(
      glm::mat4{}, SceneData{}, glm::vec3{}, {});
  scenev2Test.updateViewAspectRatio(1.0f);
  scenev2Test.updateUserControlData(UserControlData{});
  scenev2Test.updatePosition(1.0);

  return 0;
}

const float viewAspectRatio(const int& width, const int& height) {
  if (height == 0) throw std::runtime_error("height is 0");
  return static_cast<float>(width) / height;
}

std::vector<WallComponent> generateWall() {
  std::vector<WallComponent> wallComponents;
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 10; j++) {
      wallComponents.emplace_back(
          WallComponent{glm::vec3{-0.9 + 0.2 * j, 0.2 + i * 0.4, -1}, false});
      wallComponents.emplace_back(
          WallComponent{glm::vec3{-0.9 + 0.2 * j, 0.2 + i * 0.4, 1}, false});
      wallComponents.emplace_back(
          WallComponent{glm::vec3{-1, 0.2 + i * 0.4, -0.9 + 0.2 * j}, true});
      wallComponents.emplace_back(
          WallComponent{glm::vec3{1, 0.2 + i * 0.4, -0.9 + 0.2 * j}, true});
    }
  }
  return wallComponents;
}

std::vector<FloorComponent> generateFloor() {
  std::vector<FloorComponent> floorComponents;
  for (size_t i = 0; i < 10; i++) {
    for (size_t j = 0; j < 10; j++) {
      floorComponents.emplace_back(
          FloorComponent{glm::vec3{-0.9 + 0.2 * i, 0, -0.9 + 0.2 * j}});
    }
  }
  return floorComponents;
}

std::vector<CeilingComponent> generateCeiling() {
  std::vector<CeilingComponent> ceilingComponents;
  for (size_t i = 0; i < 10; i++) {
    for (size_t j = 0; j < 10; j++) {
      ceilingComponents.emplace_back(
          CeilingComponent{glm::vec3{-0.9 + 0.2 * i, 1.2, -0.9 + 0.2 * j}});
    }
  }
  return ceilingComponents;
}

std::vector<SphereComponent> generateSpheres() {
  std::vector<SphereComponent> sphereComponents;
  for (int i = 0; i < 100; i++) {
    sphereComponents.emplace_back(
        SphereComponent{generateRandomAnimatedSphereDataV2()});
  }
  return sphereComponents;
}

std::vector<CameraComponent> generateDefaultCamera(const glm::vec3& position) {
  return std::vector<CameraComponent>{CameraComponent{position}};
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) noexcept {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  WindowUserData* userData =
      static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));

  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    // growQuadTree(windowUserData->quadTree, window, true);
    // growQuadTree(windowUserData->quadTree, window, false);
    const glm::vec3 position = generateRandomPosition();
    growQuadTreeWithInheritedController(userData->quadTree, window);
    growQuadTreeWithNewController(userData->quadTree, window, position);
    userData->cameras.emplace_back(CameraComponent{position});
  }

  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
    if (shrinkQuadTree(userData->quadTree) &&
        shrinkQuadTree(userData->quadTree)) {
      userData->cameras.pop_back();
    }
  }

  if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    userData->isBirdView = !userData->isBirdView;
}

static const glm::vec3 generateRandomPosition() noexcept {
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_real_distribution<> locationDistribution(-0.9, 0.9);
  std::uniform_real_distribution<> heightDistribution(0.2, 0.9);

  const glm::vec3 position{locationDistribution(engine),
                           heightDistribution(engine),
                           locationDistribution(engine)};
  return position;
}
