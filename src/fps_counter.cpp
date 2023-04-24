#include "fps_counter.h"

FpsCounter::FpsCounter(GLFWwindow* window, const char* title,
                       double initTimestamp)
    : referenceTimestamp(initTimestamp), window(window),
      title(std::string{title}) {}

void FpsCounter::updateFramerate(double currentTimestamp) {
  if (double delta = currentTimestamp - referenceTimestamp; delta < 1.0)
      [[likely]] {
    frameCount++;
  } else [[unlikely]] {
    double framerate = frameCount / delta;
    referenceTimestamp = currentTimestamp;
    glfwSetWindowTitle(
        window,
        std::string{title + std::format(" [{:.2f} fps]", framerate)}.c_str());
    frameCount = 0;
  }
};
