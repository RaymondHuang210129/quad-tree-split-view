#pragma once
#include <array>
#include <random>
#include <tuple>
#include <variant>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "axes.h"
#include "camera.h"
#include "floor.h"
#include "grid.h"
#include "light_source.h"
#include "quad_tree.h"
#include "sphere.h"
#include "user_control.h"
#include "wall.h"

using StaticComponent =
    std::variant<AxesComponent, GridComponent, LightSourceComponent>;
using LightingComponent = std::variant<FloorComponent, WallComponent>;

struct AnimatedSphereData {
  glm::vec3 originalPosition;
  glm::vec3 movingScale;
  float cycleOffset;
  SphereData sphereData;
  float speed;
};

struct SceneData {
  std::vector<AnimatedSphereData> spheres;
  bool isBirdView;
};

class Scene {
public:
  Scene(const float& viewAspectRatio);
  void render(const glm::mat4& view, const SceneData& data,
              const glm::vec3& viewPosition,
              std::vector<std::shared_ptr<QuadTreeNode>> treeLeafs) const;
  void updateViewAspectRatio(const float& viewAspectRatio);

private:
  glm::mat4 proj{};
  std::vector<StaticComponent> staticComponents{AxesComponent{},
                                                GridComponent{}};
  std::vector<LightingComponent> lightingComponents{};
  std::vector<FloorComponent> cellingComponents{};
  mutable std::vector<SphereComponent> sphereComponents{};
  LightSourceComponent lightSource{glm::vec3{0.3f, 0.99f, 0.8f}};

  void addWalls();
  void addFloor();
  void addCeiling();
  void renderSpheres(const glm::mat4& view, const glm::vec3& viewPosition,
                     const std::vector<AnimatedSphereData>& data) const;
};

class SceneController {
public:
  SceneController();
  void updateSceneData(const bool& isBirdView, double currentTimestamp);
  const SceneData& sceneData() const;

private:
  SceneData data{.isBirdView{false}};

  float degreeCycleCounter{};
};

template <typename T> struct has_render_void_method {
  static constexpr bool value = requires(T t) { t.render(); };
};

template <typename T>
inline constexpr bool has_render_void_method_v =
    has_render_void_method<T>::value;

template <typename T> struct has_render_int_method {
  static constexpr bool value = requires(T t) {
    t.render(std::declval<int>());
  };
};

template <typename T>
inline constexpr bool has_render_int_method_v = has_render_int_method<T>::value;

template <typename T> struct has_render_int_int_method {
  static constexpr bool value = requires(T t) {
    t.render(std::declval<int>(), std::declval<int>());
  };
};

template <typename T>
inline constexpr bool has_render_int_int_method_v =
    has_render_int_int_method<T>::value;

template <typename T> struct has_render_mat4_mat4_method {
  static constexpr bool value = requires(T t) {
    t.render(std::declval<glm::mat4>(), std::declval<glm::mat4>());
  };
};

template <typename T>
inline constexpr bool HasPlainRender = has_render_mat4_mat4_method<T>::value;

template <typename T> struct has_render_mat4_mat4_vec3_vec3_method {
  static constexpr bool value = requires(T t) {
    t.render(std::declval<glm::mat4>(), std::declval<glm::mat4>(),
             std::declval<glm::vec3>(), std::declval<glm::vec3>());
  };
};

template <typename T>
inline constexpr bool HasPhongRender =
    has_render_mat4_mat4_vec3_vec3_method<T>::value;

template <typename T>
struct has_render_mat4_mat4_double_double_vec3_vec3_method {
  static constexpr bool value = requires(T t) {
    t.render(std::declval<glm::mat4>(), std::declval<glm::mat4>(),
             std::declval<double>(), std::declval<double>(),
             std::declval<glm::vec3>(), std::declval<glm::vec3>());
  };
};

template <typename T>
inline constexpr bool HasPhongUserControlRender =
    has_render_mat4_mat4_double_double_vec3_vec3_method::value;

template <typename... Ts>
concept Renderable = ((has_render_void_method_v<Ts> ||
                       has_render_int_method_v<Ts> ||
                       has_render_int_int_method_v<Ts> || HasPlainRender<Ts> ||
                       HasPhongRender<Ts> ||
                       HasPhongUserControlRender<Ts>)&&...);

template <typename T> struct has_updateUserControlData_method {
  static constexpr bool value = requires() {
    T::updateUserControlData(std::declval<UserControlData>());
  };
};

template <typename T>
inline constexpr bool HasUserControl =
    has_updateUserControlData_method<T>::value;

template <typename T> struct has_updatePosition_method {
  static constexpr bool value = requires(T t) {
    t.updatePosition(std::declval<double>());
  };
};

template <typename T>
inline constexpr bool IsMovable = has_updatePosition_method<T>::value;

template <typename...> inline constexpr bool is_unique = std::true_type{};

template <typename T, typename... Rest>
inline constexpr bool is_unique<T, Rest...> =
    std::bool_constant<(!std::is_same_v<T, Rest> && ...) &&
                       is_unique<Rest...>>{};

template <typename... Ts>
concept UniqueType = is_unique<Ts...>;

template <typename... Components>
requires Renderable<Components...> &&
    UniqueType<Components...> /* Remove Renderable also works but generates
                            uglier error message */
class SceneV2 {

  using VectorsTuple = std::tuple<std::vector<Components>...>;

  template <int N, typename T>
  struct IsNthElementTypeT
      : std::is_same<
            T, typename std::tuple_element_t<N, VectorsTuple>::value_type> {};

  template <int N, class T, class Tuple, bool Match = false>
  struct MatchingField {
    static std::vector<T>& get(Tuple& tuple) {
      return MatchingField<N + 1, T, Tuple,
                           IsNthElementTypeT<N + 1, T>::value>::get(tuple);
    }
  };

  template <int N, class T, class Tuple>
  struct MatchingField<N, T, Tuple, true> {
    static std::vector<T>& get(Tuple& tuple) { return std::get<N>(tuple); }
  };

  template <typename TVec>
  static void _render(const TVec& vector, const glm::mat4& view,
                      const glm::mat4& proj, const SceneData& data,
                      const glm::vec3& viewPosition,
                      const LightSourceComponent& lightSource) {
    for (const auto& component : vector) {
      if constexpr (has_render_void_method_v<std::remove_cv_t<
                        std::remove_reference_t<decltype(component)>>>) {
        component.render();
      } else if constexpr (has_render_int_method_v<std::remove_cv_t<
                               std::remove_reference_t<decltype(component)>>>) {
        component.render(1);
      } else if constexpr (has_render_int_int_method_v<std::remove_cv_t<
                               std::remove_reference_t<decltype(component)>>>) {
        component.render(1, 1);
      } else if constexpr (HasPlainRender<std::remove_cv_t<
                               std::remove_reference_t<decltype(component)>>>) {
        component.render(view, proj);
      } else if constexpr (HasPhongRender<std::remove_cv_t<
                               std::remove_reference_t<decltype(component)>>>) {
        component.render(view, proj, viewPosition, lightSource.position());
      } else if constexpr (HasPhongUserControlRender<std::remove_cv_t<
                               std::remove_reference_t<decltype(component)>>>) {
        component.render(view, proj, viewPosition, lightSource.position());
      } else {
        std::cout << "error" << std::endl;
      }
    }
  };

  template <typename TVec>
  static void _updatePosition(TVec& vector, const double& currentTimestamp) {
    for (auto& component : vector) {
      component.updatePosition(currentTimestamp);
    }
  };

  template <int I> struct RenderHelper {
    inline static void render(VectorsTuple& vectorsTuple, const glm::mat4& view,
                              const glm::mat4& proj, const SceneData& data,
                              const glm::vec3& viewPosition,
                              const LightSourceComponent& lightSource) {
      const auto& vector = std::get<I>(vectorsTuple);
      _render<std::tuple_element_t<I, VectorsTuple>>(vector, view, proj, data,
                                                     viewPosition, lightSource);
      if constexpr (I + 1 < std::tuple_size_v<VectorsTuple>) {
        RenderHelper<I + 1>::render(vectorsTuple, view, proj, data,
                                    viewPosition, lightSource);
      }
    }
  };

  template <int I> struct UpdateUserDataHelper {
    inline static void updateUserControlData(const UserControlData& userData) {
      if constexpr (HasUserControl<typename std::tuple_element_t<
                        I, VectorsTuple>::value_type>) {
        typename std::tuple_element_t<
            I, VectorsTuple>::value_type::updateUserControlData(userData);
      }
      if constexpr (I + 1 < std::tuple_size_v<VectorsTuple>) {
        UpdateUserDataHelper<I + 1>::updateUserControlData(userData);
      }
    }
  };

  template <int I> struct UpdatePositionHelper {
    inline static void updatePosition(VectorsTuple& vectorsTuple,
                                      const double& currentTimestamp) {
      if constexpr (IsMovable<typename std::tuple_element_t<
                        I, VectorsTuple>::value_type>) {
        auto& vector = std::get<I>(vectorsTuple);
        _updatePosition<std::tuple_element_t<I, VectorsTuple>>(
            vector, currentTimestamp);
      }
      if constexpr (I + 1 < std::tuple_size_v<VectorsTuple>) {
        UpdatePositionHelper<I + 1>::updatePosition(vectorsTuple,
                                                    currentTimestamp);
      }
    }
  };

public:
  SceneV2(const float& viewAspectRatio, std::vector<Components>... typeVector)
      : vectorsTuple(typeVector...) {
    updateViewAspectRatio(viewAspectRatio);
  };

  template <typename T> std::vector<T>& getVector() {
    return MatchingField<0, T, VectorsTuple,
                         IsNthElementTypeT<0, T>::value>::get(vectorsTuple);
  };

  template <typename T> inline void addComponent(T&& element) {
    std::get<std::vector<T>>(vectorsTuple)
        .emplace_back(std::forward<T>(element));
  };

  // todo: revmove treeLeafs when main function is reimplemented
  template <typename T, typename... Rest>
  inline void render(const glm::mat4& view, const SceneData& data,
                     const glm::vec3& viewPosition,
                     std::vector<std::shared_ptr<QuadTreeNode>> treeLeafs) {
    const auto& vector = std::get<std::vector<T>>(vectorsTuple);
    _render<std::vector<T>>(vector, view, proj, data, viewPosition,
                            lightSource);
    if constexpr (sizeof...(Rest) > 0) {
      render<Rest...>(view, data, viewPosition, treeLeafs);
    }
  };

  // todo: revmove treeLeafs when main function is reimplemented
  inline void renderAll(const glm::mat4& view, const SceneData& data,
                        const glm::vec3& viewPosition,
                        std::vector<std::shared_ptr<QuadTreeNode>> treeLeafs) {
    RenderHelper<0>::render(vectorsTuple, view, proj, data, viewPosition,
                            lightSource);
  };

  inline void updateUserControlData(const UserControlData& userControlData) {
    UpdateUserDataHelper<0>::updateUserControlData(userControlData);
  };

  inline void updatePosition(const double& currentTimestamp) {
    UpdatePositionHelper<0>::updatePosition(vectorsTuple, currentTimestamp);
  };

  void updateViewAspectRatio(const float& viewAspectRatio) {
    static float currentAspectRatio;
    if (currentAspectRatio != viewAspectRatio) [[unlikely]]
      proj = glm::mat4{glm::perspective(glm::radians(45.0f), viewAspectRatio,
                                        0.01f, 100.0f)};
  };

private:
  VectorsTuple vectorsTuple{};
  // todo (far away): make lightsource component dynamic
  LightSourceComponent lightSource{glm::vec3{0.3f, 0.99f, 0.8f}};
  glm::mat4 proj{};
};
