#include "user_control.h"

FirstPersonController::FirstPersonController(GLFWwindow* window,
                                             const glm::vec3& position)
    : window{window}, _position{position} {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
};

const glm::mat4& FirstPersonController::view() const { return _view; };

const glm::vec3& FirstPersonController::position() const { return _position; };

const double FirstPersonController::horizontalAngleRadians() {
  return userData.horizontalAngleRadians;
};

const double FirstPersonController::verticalAngleRadians() {
  return userData.verticalAngleRadians;
};

void FirstPersonController::updateView() {
  double xPos{}, yPos{};
  glfwGetCursorPos(window, &xPos, &yPos);
  int windowWidth{}, windowHeight{};
  glfwGetWindowSize(window, &windowWidth, &windowHeight);

  const auto xDelta{0.5 * windowWidth - xPos};
  const auto yDelta{0.5 * windowHeight - yPos};

  float mouseSpeed{0.005f};

  userData.horizontalAngleRadians = mouseSpeed * xDelta;
  userData.verticalAngleRadians = mouseSpeed * yDelta;

  if (userData.verticalAngleRadians >= (80.0 * M_PI / 180.0))
    userData.verticalAngleRadians = 80.0 * M_PI / 180.0;
  if (userData.verticalAngleRadians <= (-70.0 * M_PI / 180.0))
    userData.verticalAngleRadians = -70.0 * M_PI / 180.0;

  const glm::vec3 _direction{std::cos(userData.verticalAngleRadians) *
                                 std::sin(userData.horizontalAngleRadians),
                             std::sin(userData.verticalAngleRadians),
                             std::cos(userData.verticalAngleRadians) *
                                 std::cos(userData.horizontalAngleRadians)};

  const glm::vec3 _right{
      glm::vec3{std::sin(userData.horizontalAngleRadians - 3.14f / 2.0f), 0.0f,
                std::cos(userData.horizontalAngleRadians - 3.14f / 2.0f)}};

  const glm::vec3 up{glm::cross(_right, _direction)};

  _view = glm::lookAt(_position, _position + _direction, up);
};

const UserControlData& FirstPersonController::getUserData() { return userData; }
