#include <array>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>

import raii_glfw;
import scene;
import user_control;

void framebufferSizeCallback(GLFWwindow* window, int width,
                             int height) noexcept;
const float viewAspectRatio() noexcept;

struct WindowUserData {
  Scene* scene;
};

int main() {
  const RaiiGlfw raiiGlfw{};

  GLFWwindow* window =
      glfwCreateWindow(800, 600, "Mini Portal", nullptr, nullptr);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  Scene scene{viewAspectRatio()};
  SceneController scene_controller{};

  WindowUserData userData{&scene};
  glfwSetWindowUserPointer(window, &userData);

  FirstPersonController firstPersonController{window};

  while (!glfwWindowShouldClose(window)) {
    scene_controller.updateSceneData();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    firstPersonController.updateUserInputs(window);
    scene.render(firstPersonController.view(), scene_controller.sceneData(),
                 firstPersonController.position());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width,
                             int height) noexcept {
  glViewport(0, 0, width, height);

  WindowUserData* userData =
      static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));
  userData->scene->updateViewAspectRatio(viewAspectRatio());
}

const float viewAspectRatio() noexcept {
  std::array<GLint, 4> viewport{};
  glGetIntegerv(GL_VIEWPORT, viewport.data());
  return {static_cast<float>(viewport.at(2) - viewport.at(0)) /
          (viewport.at(3) - viewport.at(1))};
}
