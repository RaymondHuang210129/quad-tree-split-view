#include <tuple>
#include <vector>

template <typename... Type> class Scene {

  using VectorsTuple = std::tuple<std::vector<Type>...>;

  template <class T1, class T2> struct SameType {
    static constexpr bool value = std::is_same_v<T1, T2>;
  };

public:
  Scene(std::vector<Type>... typeVector) : vectorsTuple(typeVector...){};

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

  template <typename T> std::vector<T>& getVector() {
    return MatchingField<0, T, VectorsTuple, VectorOfType<0, T>::value>::get(
        vectorsTuple);
  }

private:
  VectorsTuple vectorsTuple;
};

void test() {
  Scene<int, float, double> scene{{std::vector<int>{1, 2, 3}},
                                  {std::vector<float>{1.0f, 2.0f, 3.0f}},
                                  {std::vector<double>{1.0, 2.0, 3.0}}};

  auto i = scene.getVector<int>();
}
