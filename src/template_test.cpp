#include <iostream>
#include <tuple>
#include <vector>

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

template <typename... Args>
concept Renderable = ((has_render_void_method_v<Args> ||
                       has_render_int_method_v<Args> ||
                       has_render_int_int_method_v<Args>)&&...);

template <typename... Type>
requires Renderable<Type...> /* Removing this also works but generates uglier
                                error message */
class Scene {

  using VectorsTuple = std::tuple<std::vector<Type>...>;

  template <class T1, class T2> struct SameType {
    static constexpr bool value = std::is_same_v<T1, T2>;
  };

  template <int N, typename T>
  struct VectorOfType
      : SameType<
            T, typename std::tuple_element<N, VectorsTuple>::type::value_type> {
  };

  template <int N, class T, class Tuple, bool Match = false>
  struct MatchingField {
    static std::vector<T>& get(Tuple& tuple) {
      return MatchingField<N + 1, T, Tuple, VectorOfType<N + 1, T>::value>::get(
          tuple);
    }
  };

  template <int N, class T, class Tuple>
  struct MatchingField<N, T, Tuple, true> {
    static std::vector<T>& get(Tuple& tuple) { return std::get<N>(tuple); }
  };

  template <int I> struct Renderer {
    static void render(VectorsTuple& vectorsTuple) {
      auto& vector = std::get<I>(vectorsTuple);
      for (auto element : vector) {
        if constexpr (has_render_void_method_v<
                          std::remove_reference_t<decltype(element)>>) {
          element.render();
        } else if constexpr (has_render_int_method_v<
                                 std::remove_reference_t<decltype(element)>>) {
          element.render(1);
        } else if constexpr (has_render_int_int_method_v<
                                 std::remove_reference_t<decltype(element)>>) {
          element.render(1, 1);
        } else {
          std::cout << "error" << std::endl;
        }
      }
      if constexpr (I < std::tuple_size_v<VectorsTuple> - 1) {
        Renderer<I + 1>{}.render(vectorsTuple);
      }
    }
  };

public:
  Scene(std::vector<Type>... typeVector) : vectorsTuple(typeVector...){};

  template <typename T> std::vector<T>& getVector() {
    return MatchingField<0, T, VectorsTuple, VectorOfType<0, T>::value>::get(
        vectorsTuple);
  };

  void render() { Renderer<0>{}.render(vectorsTuple); };

private:
  VectorsTuple vectorsTuple;
};

class Component1 {
public:
  void render() { std::cout << "Component1 is rendered" << std::endl; };
};

class Component2 {
public:
  void render(int i) { std::cout << "Component2 is rendered" << std::endl; };
};

class Component3 {
public:
  void render(int i, int j) {
    std::cout << "Component3 is rendered" << std::endl;
  };
};

int test() {

  Scene<Component1, Component2, Component3> scene{
      {std::vector<Component1>{Component1{}, Component1{}}},
      {std::vector<Component2>{Component2{}}},
      {std::vector<Component3>{Component3{}, Component3{}, Component3{}}}};

  scene.render();

  std::cout << has_render_void_method_v<Component1> << std::endl;

  return 0;
}
