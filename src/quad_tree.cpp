#include "quad_tree.h"

static const size_t getParentIdx(const size_t& idx) noexcept;
static const size_t getDepth(const size_t& idx) noexcept;
static const glm::vec3 generateRandomPosition() noexcept;

std::vector<std::shared_ptr<QuadTreeNode>>
getQuadTreeLeaves(std::vector<std::shared_ptr<QuadTreeNode>>& tree) {
  std::vector<std::shared_ptr<QuadTreeNode>> leaves{};
  for (size_t i = tree.size() / 2; i < tree.size(); i++) {
    leaves.push_back(tree.at(i));
  }

  return leaves;
};

bool shrinkQuadTree(std::vector<std::shared_ptr<QuadTreeNode>>& tree) {
  if (tree.size() > 1) {
    tree.pop_back();
    return true;
  }
  return false;
}

void growQuadTree(std::vector<std::shared_ptr<QuadTreeNode>>& tree,
                  GLFWwindow* window, bool inheritParentController) {
  if (tree.empty()) return;

  const auto newIdx = tree.size();
  const auto& parent = tree[getParentIdx(newIdx)];

  std::shared_ptr<FirstPersonController> controller(nullptr);

  if (inheritParentController)
    controller = std::make_shared<FirstPersonController>(
        window, generateRandomPosition());
  else controller = parent->firstPersonController;

  if (getDepth(newIdx) % 2 != 0) {
    if (newIdx % 2 != 0) {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width / 2, parent->height, parent->x, parent->y, controller));
    } else {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width / 2, parent->height, parent->x + parent->width / 2,
          parent->y, controller));
    }
  } else {
    if (newIdx % 2 != 0) {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width, parent->height / 2, parent->x, parent->y, controller));
    } else {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width, parent->height / 2, parent->x,
          parent->y + parent->height / 2, controller));
    }
  }
}

void growQuadTreeWithNewController(
    std::vector<std::shared_ptr<QuadTreeNode>>& tree, GLFWwindow* window,
    const glm::vec3 position) {
  const auto newIdx = tree.size();
  const auto& parent = tree[getParentIdx(newIdx)];

  const std::shared_ptr<FirstPersonController> controller =
      std::make_shared<FirstPersonController>(window, position);

  // todo: extract this as a local function
  if (getDepth(newIdx) % 2 != 0) {
    if (newIdx % 2 != 0) {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width / 2, parent->height, parent->x, parent->y, controller));
    } else {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width / 2, parent->height, parent->x + parent->width / 2,
          parent->y, controller));
    }
  } else {
    if (newIdx % 2 != 0) {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width, parent->height / 2, parent->x, parent->y, controller));
    } else {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width, parent->height / 2, parent->x,
          parent->y + parent->height / 2, controller));
    }
  }
}

void growQuadTreeWithInheritedController(
    std::vector<std::shared_ptr<QuadTreeNode>>& tree, GLFWwindow* window) {
  const auto newIdx = tree.size();
  const auto& parent = tree[getParentIdx(newIdx)];

  std::shared_ptr<FirstPersonController> controller =
      parent->firstPersonController;

  if (getDepth(newIdx) % 2 != 0) {
    if (newIdx % 2 != 0) {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width / 2, parent->height, parent->x, parent->y, controller));
    } else {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width / 2, parent->height, parent->x + parent->width / 2,
          parent->y, controller));
    }
  } else {
    if (newIdx % 2 != 0) {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width, parent->height / 2, parent->x, parent->y, controller));
    } else {
      tree.push_back(std::make_shared<QuadTreeNode>(
          parent->width, parent->height / 2, parent->x,
          parent->y + parent->height / 2, controller));
    }
  }
}

static const size_t getParentIdx(const size_t& idx) noexcept {
  return (idx + 1) / 2 - 1;
}

static const size_t getDepth(const size_t& idx) noexcept {
  return static_cast<size_t>(std::floor(std::log2(idx + 1)));
}

static const glm::vec3 generateRandomPosition() noexcept {
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_real_distribution<> locationDistribution(-0.9, 0.9);
  std::uniform_real_distribution<> heightDistribution(0.2, 0.9);

  const glm::vec3 position{locationDistribution(engine),
                           heightDistribution(engine),
                           locationDistribution(engine)};
  return position;
}
