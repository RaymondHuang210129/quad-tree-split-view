#include "global_timer.h"

GlobalTimer::GlobalTimer() {
  epochTime = glfwGetTime();
  currentTime = 0;
}

double GlobalTimer::getCurrentTime() { return currentTime; }

void GlobalTimer::updateTime() { currentTime = glfwGetTime() - epochTime; }
