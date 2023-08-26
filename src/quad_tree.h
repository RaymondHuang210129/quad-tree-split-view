#pragma once
#include <cmath>
#include <random>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include "user_control.h"

class QuadTreeNode {
public:
  QuadTreeNode(float _width, float _height, float _x, float _y,
               std::shared_ptr<FirstPersonController> _firstPersonController)
      : width(_width), height(_height), x(_x), y(_y),
        firstPersonController(_firstPersonController) {}
  float width;
  float height;
  float x;
  float y;
  std::shared_ptr<FirstPersonController> firstPersonController;
};

std::vector<std::shared_ptr<QuadTreeNode>>
getQuadTreeLeaves(std::vector<std::shared_ptr<QuadTreeNode>>& tree);
bool shrinkQuadTree(std::vector<std::shared_ptr<QuadTreeNode>>& tree);
void growQuadTree(std::vector<std::shared_ptr<QuadTreeNode>>& tree,
                  GLFWwindow* window, bool inheritParentController);
void growQuadTreeWithNewController(
    std::vector<std::shared_ptr<QuadTreeNode>>& tree, GLFWwindow* window,
    const glm::vec3 position);
void growQuadTreeWithInheritedController(
    std::vector<std::shared_ptr<QuadTreeNode>>& tree, GLFWwindow* window);
static const size_t getParentIdx(const size_t& idx) noexcept;
static const size_t getDepth(const size_t& idx) noexcept;
static const glm::vec3 generateRandomPosition() noexcept;
