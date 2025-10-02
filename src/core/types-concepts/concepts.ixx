export module concepts;

import std;

export
{
  template<typename T>
  concept FloatingArithmetic = std::floating_point<std::remove_cvref_t<T>>;

  template<typename T>
  concept IntegralArithmetic = std::integral<std::remove_cvref_t<T>>;

  template<typename T>
  concept UnsignedIntegralArithmetic = std::unsigned_integral<std::remove_cvref_t<T>>;

  template<typename T>
  concept Arithmetic = IntegralArithmetic<T> || FloatingArithmetic<T>;

  template<typename T>
  concept Vector3Like = requires (const T & v)
  {
    { v[0] } -> Arithmetic;
    { v[1] } -> Arithmetic;
    { v[2] } -> Arithmetic;
  };

  template<typename V>
  using ScalarOf = std::remove_cvref_t<decltype(std::declval<V&>()[0])>;

  template<typename A, typename B>
  using CommonTypeOfScalars = std::common_type_t<ScalarOf<A>, ScalarOf<B>>;

  template<typename A, typename B>
  concept Vec3Common = Vector3Like<A> && Vector3Like<B> && requires { typename CommonTypeOfScalars<A, B>; } && std::convertible_to<ScalarOf<A>, CommonTypeOfScalars<A, B>> && std::convertible_to<ScalarOf<B>, CommonTypeOfScalars<A, B>> && Arithmetic<CommonTypeOfScalars<A, B>>;

  template<std::size_t N>
  concept Arity = (N == 2) || (N == 3) || (N == 4);

  template<typename... Ts>
  concept FPCommon = std::floating_point<std::common_type_t<Ts...>>;
}
