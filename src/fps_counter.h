#pragma once
#include <format>
#include <GLFW/glfw3.h>
#include <string>

class FpsCounter {
public:
  FpsCounter(GLFWwindow* window, const char* title, double initTimestamp);
  void updateFramerate(double currentTimestamp);

private:
  GLFWwindow* window{};
  std::string title{};
  double referenceTimestamp{};
  int frameCount{};
};
