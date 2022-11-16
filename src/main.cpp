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
const float viewAspectRatio(const int& width, const int& height);

struct WindowUserData {
  int width;
  int height;
  Scene* scene;
};

const int NUM_CAMERAS{2};

int main() {
  const RaiiGlfw raiiGlfw{};

  const int defaultWidth{1080};
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

  WindowUserData userData{defaultWidth, defaultHeight, &scene};
  glfwSetWindowUserPointer(window, &userData);

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  FirstPersonController firstPersonController{window};
  const glm::mat4 fixedView{
      glm::lookAt(glm::vec3{0.3, 0.6, 0.9}, {0, 0, 0}, {0, 1, 0})};

  while (!glfwWindowShouldClose(window)) {
    sceneController.updateSceneData();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, userData.width / NUM_CAMERAS, userData.height);
    firstPersonController.updateUserInputs(window);
    scene.render(firstPersonController.view(), sceneController.sceneData(),
                 firstPersonController.position());

    glViewport(userData.width / NUM_CAMERAS, 0, userData.width / NUM_CAMERAS,
               userData.height);
    scene.render(fixedView, sceneController.sceneData(),
                 firstPersonController.position());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width,
                             int height) noexcept {
  WindowUserData* userData =
      static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));
  userData->width = width;
  userData->height = height;
  userData->scene->updateViewAspectRatio(viewAspectRatio(width, height));
}

const float viewAspectRatio(const int& width, const int& height) {
  if (NUM_CAMERAS == 0) throw std::runtime_error("NUM_CAMERAS is 0");
  if (height == 0) throw std::runtime_error("height is 0");
  return (static_cast<float>(width) / NUM_CAMERAS) / height;
}
