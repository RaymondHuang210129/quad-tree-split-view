#include <array>
#include <iostream>
#include <random>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>

import raii_glfw;
import scene;
import user_control;
import quad_tree;

const float viewAspectRatio(const int& width, const int& height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) noexcept;

struct WindowUserData {
  std::vector<QuadTreeNode> quadTree;
  bool isBirdView;
};

int main() {
  const RaiiGlfw raiiGlfw{};

  const int defaultWidth{720};
  const int defaultHeight{720};

  GLFWwindow* window = glfwCreateWindow(defaultWidth, defaultHeight,
                                        "Mini Portal", nullptr, nullptr);
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

  Scene scene{viewAspectRatio(defaultWidth, defaultHeight)};
  SceneController sceneController{};

  WindowUserData userData{
      .quadTree{
          {.width = 1,
           .height = 1,
           .x = 0,
           .y = 0,
           .firstPersonController =
               FirstPersonController{window, glm::vec3{0.0f, 0.2f, 0.8f}}}},
      .isBirdView{false}};
  glfwSetWindowUserPointer(window, &userData);

  glfwSetKeyCallback(window, keyCallback);

  while (!glfwWindowShouldClose(window)) {
    sceneController.updateSceneData(userData.isBirdView);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int windowWidth{}, windowHeight{};
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    if (userData.isBirdView) {
      glViewport(0, 0, windowWidth, windowHeight);
      scene.updateViewAspectRatio(viewAspectRatio(windowWidth, windowHeight));
      scene.render(glm::lookAt({1.25, 4, 1.25}, glm::vec3{0}, {0, 1, 0}),
                   sceneController.sceneData(), glm::vec3{1.5, 1.5, 1.5});

    } else {
      for (auto& leaf : getQuadTreeLeaves(userData.quadTree)) {
        leaf.firstPersonController.updateView();

        glViewport(static_cast<GLint>(leaf.x * windowWidth),
                   static_cast<GLint>(leaf.y * windowHeight),
                   static_cast<GLsizei>(leaf.width * windowWidth),
                   static_cast<GLsizei>(leaf.height * windowHeight));
        scene.updateViewAspectRatio(
            viewAspectRatio(static_cast<int>(leaf.width * windowWidth),
                            static_cast<int>(leaf.height * windowHeight)));
        scene.render(leaf.firstPersonController.view(),
                     sceneController.sceneData(),
                     leaf.firstPersonController.position());
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}

const float viewAspectRatio(const int& width, const int& height) {
  if (height == 0) throw std::runtime_error("height is 0");
  return static_cast<float>(width) / height;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) noexcept {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  WindowUserData* userData =
      static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));

  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    growQuadTree(userData->quadTree, window);
    growQuadTree(userData->quadTree, window);
  }

  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
    shrinkQuadTree(userData->quadTree);
    shrinkQuadTree(userData->quadTree);
  }

  if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    userData->isBirdView = !userData->isBirdView;
}
