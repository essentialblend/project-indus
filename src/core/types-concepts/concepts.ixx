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

  template<typename V>
  using ScalarOf = std::remove_cvref_t<decltype(std::declval<V&>()[0])>;

  template<typename T>
  concept ScalarLike = requires(T a, T b) 
  {
      { T{ a } };
      { -a };
      { a + b } -> std::same_as<std::remove_cvref_t<T>>;
      { a - b } -> std::same_as<std::remove_cvref_t<T>>;
      { a * b } -> std::same_as<std::remove_cvref_t<T>>;
      { a / b } -> std::same_as<std::remove_cvref_t<T>>;
  };

  template<typename T>
  concept IntervalScalarLike = requires
  {
    requires FloatingArithmetic<std::remove_cvref_t<decltype(std::declval<T>().getLower())>>;
    requires FloatingArithmetic<std::remove_cvref_t<decltype(std::declval<T>().getUpper())>>;
  };

  template<typename T>
  concept Arithmetic = IntegralArithmetic<T> || FloatingArithmetic<T>;

  template<typename T>
  concept Vector3Like = requires (const T & v)
  {
    { v[0] } -> ScalarLike;
    { v[1] } -> ScalarLike;
    { v[2] } -> ScalarLike;
  };

  template<typename V>
  concept FloatOnlyVector3Like = Vector3Like<V> && FloatingArithmetic<ScalarOf<V>>;

  template<typename A, typename B>
  using CommonTypeOfScalars = std::common_type_t<ScalarOf<A>, ScalarOf<B>>;

  template<typename A, typename B>
  concept IntervalOnlyVec3Common = Vector3Like<A> && Vector3Like<B> && requires { typename CommonTypeOfScalars<A, B>; } && std::convertible_to<ScalarOf<A>, CommonTypeOfScalars<A, B>> && std::convertible_to<ScalarOf<B>, CommonTypeOfScalars<A, B>> && IntervalScalarLike<CommonTypeOfScalars<A, B>>;

  template<typename A, typename B>
  concept FloatOnlyVec3Common = FloatOnlyVector3Like<A> && FloatOnlyVector3Like<B> && requires { typename CommonTypeOfScalars<A, B>; } && FloatingArithmetic<CommonTypeOfScalars<A, B>>;

  template<std::size_t N>
  concept Arity = (N == 2) || (N == 3) || (N == 4);

  template<typename... Ts>
  concept FPCommon = std::floating_point<std::common_type_t<Ts...>>;
}
