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
  static constexpr bool value = requires(T t) { t.render(1); };
};

template <typename T>
inline constexpr bool has_render_int_method_v = has_render_int_method<T>::value;

template <typename T> struct has_render_int_int_method {
  static constexpr bool value = requires(T t) { t.render(1, 1); };
};

template <typename T>
inline constexpr bool has_render_int_int_method_v =
    has_render_int_int_method<T>::value;

template <typename T> struct has_render_mat4_mat4_method {
  static constexpr bool value = requires(T t) {
    t.render(glm::mat4{}, glm::mat4{});
  };
};

template <typename T>
inline constexpr bool IsStaticStillComponent =
    has_render_mat4_mat4_method<T>::value;

template <typename T> struct has_render_mat4_mat4_vec3_vec3_method {
  static constexpr bool value = requires(T t) {
    t.render(glm::mat4{}, glm::mat4{}, glm::vec3{}, glm::vec3{});
  };
};

template <typename T>
inline constexpr bool IsLightingStillComponent =
    has_render_mat4_mat4_vec3_vec3_method<T>::value;

template <typename... Ts>
concept Renderable = ((has_render_void_method_v<Ts> ||
                       has_render_int_method_v<Ts> ||
                       has_render_int_int_method_v<Ts> ||
                       IsStaticStillComponent<Ts> ||
                       IsLightingStillComponent<Ts>)&&...);

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
            T, typename std::tuple_element<N, VectorsTuple>::type::value_type> {
  };

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

  template <typename TVec> static void _render(TVec& vector) {
    for (auto& component : vector) {
      if constexpr (has_render_void_method_v<
                        std::remove_reference_t<decltype(component)>>) {
        component.render();
      } else if constexpr (has_render_int_method_v<
                               std::remove_reference_t<decltype(component)>>) {
        component.render(1);
      } else if constexpr (has_render_int_int_method_v<
                               std::remove_reference_t<decltype(component)>>) {
        component.render(1, 1);
      } else if constexpr (IsStaticStillComponent<
                               std::remove_reference_t<decltype(component)>>) {
        // todo: call render
        std::cout << "calling static component's render" << std::endl;
      } else if constexpr (IsLightingStillComponent<
                               std::remove_reference_t<decltype(component)>>) {
        // todo: call render
        std::cout << "calling lighting component's render" << std::endl;
      } else {
        std::cout << "error" << std::endl;
      }
    }
  };

  template <int I> struct RenderHelper {
    inline static void render(VectorsTuple& vectorsTuple) {
      auto& vector = std::get<I>(vectorsTuple);
      _render<std::tuple_element_t<I, VectorsTuple>>(vector);
      if constexpr (I + 1 < std::tuple_size_v<VectorsTuple>) {
        RenderHelper<I + 1>{}.render(vectorsTuple);
      }
    }
  };

public:
  SceneV2(std::vector<Components>... typeVector)
      : vectorsTuple(typeVector...){};

  template <typename T> std::vector<T>& getVector() {
    return MatchingField<0, T, VectorsTuple,
                         IsNthElementTypeT<0, T>::value>::get(vectorsTuple);
  };

  template <typename T> inline void addComponent(T&& element) {
    std::get<std::vector<T>>(vectorsTuple)
        .emplace_back(std::forward<T>(element));
  };

  template <typename T, typename... Rest> inline void render() {
    auto& vector = std::get<std::vector<T>>(vectorsTuple);
    _render<std::vector<T>>(vector);
    if constexpr (sizeof...(Rest) > 0) {
      render<Rest...>();
    }
  };

  inline void renderAll() { RenderHelper<0>{}.render(vectorsTuple); };

  void updateViewAspectRatio(const float& viewAspectRatio) {
    static float currentAspectRatio;
    if (currentAspectRatio != viewAspectRatio) [[unlikely]]
      proj = glm::mat4{glm::perspective(glm::radians(45.0f), viewAspectRatio,
                                        0.01f, 100.0f)};
  };

private:
  VectorsTuple vectorsTuple{};
  glm::mat4 proj{};
};
