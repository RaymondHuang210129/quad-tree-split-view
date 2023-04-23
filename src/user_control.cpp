#include "user_control.h"

FirstPersonController::FirstPersonController(GLFWwindow* window,
                                             const glm::vec3& position)
    : window{window}, _position{position} {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
};

const glm::mat4& FirstPersonController::view() const { return _view; };

const glm::vec3& FirstPersonController::position() const { return _position; };

const double FirstPersonController::horizontalAngleRadians() {
  return _horizontalAngleRadians;
};

const double FirstPersonController::verticalAngleRadians() {
  return _verticalAngleRadians;
};

void FirstPersonController::updateView() {
  double xPos{}, yPos{};
  glfwGetCursorPos(window, &xPos, &yPos);
  int windowWidth{}, windowHeight{};
  glfwGetWindowSize(window, &windowWidth, &windowHeight);

  const auto xDelta{0.5 * windowWidth - xPos};
  const auto yDelta{0.5 * windowHeight - yPos};

  float mouseSpeed{0.005f};

  _horizontalAngleRadians = mouseSpeed * xDelta;
  _verticalAngleRadians = mouseSpeed * yDelta;

  if (_verticalAngleRadians >= (80.0 * M_PI / 180.0))
    _verticalAngleRadians = 80.0 * M_PI / 180.0;
  if (_verticalAngleRadians <= (-70.0 * M_PI / 180.0))
    _verticalAngleRadians = -70.0 * M_PI / 180.0;

  const glm::vec3 direction{
      std::cos(_verticalAngleRadians) * std::sin(_horizontalAngleRadians),
      std::sin(_verticalAngleRadians),
      std::cos(_verticalAngleRadians) * std::cos(_horizontalAngleRadians)};

  const glm::vec3 right{
      glm::vec3{std::sin(_horizontalAngleRadians - 3.14f / 2.0f), 0.0f,
                std::cos(_horizontalAngleRadians - 3.14f / 2.0f)}};

  const glm::vec3 up{glm::cross(right, direction)};

  _view = glm::lookAt(_position, _position + direction, up);
};
