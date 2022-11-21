module;

#include <cmath>
#include <random>
#include <vector>

#include <glm/vec3.hpp>

export module quad_tree;

import user_control;

const size_t getParentIdx(const size_t& idx) noexcept;
const size_t getDepth(const size_t& idx) noexcept;
const glm::vec3 generateRandomPosition() noexcept;

export struct QuadTreeNode {
  float width;
  float height;
  float x;
  float y;
  FirstPersonController firstPersonController;
};

export std::vector<QuadTreeNode>
getQuadTreeLeaves(std::vector<QuadTreeNode>& tree) {
  std::vector<QuadTreeNode> leaves{};
  for (size_t i = tree.size() / 2; i < tree.size(); i++)
    leaves.push_back(tree.at(i));
  return leaves;
};

export void shrinkQuadTree(std::vector<QuadTreeNode>& tree) {
  if (tree.size() > 1) tree.pop_back();
}

export void growQuadTree(std::vector<QuadTreeNode>& tree, GLFWwindow* window) {
  if (tree.empty()) return;

  const FirstPersonController controller{window, generateRandomPosition()};

  const auto newIdx = tree.size();
  const auto& parent = tree[getParentIdx(newIdx)];

  if (getDepth(newIdx) % 2 != 0) {
    if (newIdx % 2 != 0) {
      tree.push_back({.width = parent.width / 2,
                      .height = parent.height,
                      .x = parent.x,
                      .y = parent.y,
                      .firstPersonController = controller});
    } else {
      tree.push_back({.width = parent.width / 2,
                      .height = parent.height,
                      .x = parent.x + parent.width / 2,
                      .y = parent.y,
                      .firstPersonController = controller});
    }
  } else {
    if (newIdx % 2 != 0) {
      tree.push_back({.width = parent.width,
                      .height = parent.height / 2,
                      .x = parent.x,
                      .y = parent.y,
                      .firstPersonController = controller});
    } else {
      tree.push_back({.width = parent.width,
                      .height = parent.height / 2,
                      .x = parent.x,
                      .y = parent.y + parent.height / 2,
                      .firstPersonController = controller});
    }
  }
}

const size_t getParentIdx(const size_t& idx) noexcept {
  return (idx + 1) / 2 - 1;
}

const size_t getDepth(const size_t& idx) noexcept {
  return static_cast<size_t>(std::floor(std::log2(idx + 1)));
}

const glm::vec3 generateRandomPosition() noexcept {
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_real_distribution<> locationDistribution(-0.9, 0.9);
  std::uniform_real_distribution<> heightDistribution(0.2, 0.9);

  while (true) {
    const glm::vec3 position{locationDistribution(engine),
                             heightDistribution(engine),
                             locationDistribution(engine)};

    // Avoid from creating position near the wall in the room
    if (position.x < 0.1 && -0.1 < position.z && position.z < 0.1) continue;

    return position;
  }
}
