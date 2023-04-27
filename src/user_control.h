#pragma once
#define _USE_MATH_DEFINES

#include <cmath>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

struct UserControlData {
  double horizontalAngleRadians;
  double verticalAngleRadians;
};

class FirstPersonController {
public:
  FirstPersonController(GLFWwindow* window, const glm::vec3& position);
  const glm::mat4& view() const;
  const glm::vec3& position() const;
  const double horizontalAngleRadians();
  const double verticalAngleRadians();
  void updateView();
  const UserControlData& getUserData();

private:
  GLFWwindow* window{};
  glm::mat4 _view{};
  glm::vec3 _position{};
  UserControlData userData{};
};
