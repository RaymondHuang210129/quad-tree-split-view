#pragma once
#include <GLFW/glfw3.h>

class GlobalTimer {
public:
  GlobalTimer();
  void updateTime();
  double getCurrentTime();

private:
  double epochTime{};
  double currentTime{};
};
