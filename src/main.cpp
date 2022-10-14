#include <array>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

import raii_glfw;
import scene;

void framebufferSizeCallback(GLFWwindow* window, int width,
                             int height) noexcept;
const float viewAspectRatio() noexcept;

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

  const Scene scene{};
  SceneData data{};

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.render(viewAspectRatio(), data);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width,
                             int height) noexcept {
  glViewport(0, 0, width, height);
}

const float viewAspectRatio() noexcept {
  std::array<GLint, 4> viewport{};
  glGetIntegerv(GL_VIEWPORT, viewport.data());
  return {static_cast<float>(viewport.at(2) - viewport.at(0)) /
          (viewport.at(3) - viewport.at(1))};
}