#pragma once
#include <format>
#include <GLFW/glfw3.h>
#include <string>

class FpsCounter {
public:
  FpsCounter(GLFWwindow* window, const char* title);
  void updateFramerate();

private:
  GLFWwindow* window{};
  std::string title{};
  double referenceTimestamp{};
  int frameCount{};
};
