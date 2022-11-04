module;

#define _USE_MATH_DEFINES

#include <cmath>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

export module user_control;

export class FirstPersonController {
public:
  FirstPersonController(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  void updateUserInputs(GLFWwindow* window) {
    updateView(window);
    onEscPressed(window);
  }

  const glm::mat4& view() { return _view; }

  const glm::vec3& position() { return _position; }

private:
  glm::mat4 _view{};
  glm::vec3 _position{glm::vec3{0.0f, 0.2f, 0.8f}};
  // TODO: Set mouse position first to initialize the view angle in center
  double horizontalAngleRadians{};
  double verticalAngleRadians{};
  float mouseSpeed{0.005f};

  void updateView(GLFWwindow* window) {
    double xPos{}, yPos{};
    int width{}, height{};
    glfwGetCursorPos(window, &xPos, &yPos);
    glfwGetWindowSize(window, &width, &height);

    glfwSetCursorPos(window, 0.5 * width, 0.5 * height);

    horizontalAngleRadians += mouseSpeed * (0.5 * width - xPos);
    verticalAngleRadians += mouseSpeed * (0.5 * height - yPos);

    if (verticalAngleRadians >= (80.0 * M_PI / 180.0)) {
      verticalAngleRadians = 80.0 * M_PI / 180.0;
    }
    if (verticalAngleRadians <= (-70.0 * M_PI / 180.0)) {
      verticalAngleRadians = -70.0 * M_PI / 180.0;
    }

    const glm::vec3 direction{
        std::cos(verticalAngleRadians) * std::sin(horizontalAngleRadians),
        std::sin(verticalAngleRadians),
        std::cos(verticalAngleRadians) * std::cos(horizontalAngleRadians)};

    const glm::vec3 right{
        glm::vec3{std::sin(horizontalAngleRadians - 3.14f / 2.0f), 0.0f,
                  std::cos(horizontalAngleRadians - 3.14f / 2.0f)}};

    const glm::vec3 up{glm::cross(right, direction)};

    _view = glm::lookAt(_position, _position + direction, up);
  }

  void onEscPressed(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }
  }
};
