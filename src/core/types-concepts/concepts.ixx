export module concepts;

import std;

export
{
  // Fundamental types
  template<typename T>
  concept FloatingArithmetic = std::floating_point<std::remove_cvref_t<T>>;

  template<typename T>
  concept IntegralArithmetic = std::integral<std::remove_cvref_t<T>>;

  template<typename T>
  concept UnsignedIntegralArithmetic = std::unsigned_integral<std::remove_cvref_t<T>>;

  template<typename T>
  concept Arithmetic = IntegralArithmetic<T> || FloatingArithmetic<T>;

  // Determine type of scalar
  template<typename V>
  using ScalarOf = std::remove_cvref_t<decltype(std::declval<V&>()[0])>;

  // A generic scalar concept (accepts both numerical types and intervals)
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

  // An Interval-only Scalar concept
  template<typename T>
  concept IntervalScalarLike = requires
  {
    requires FloatingArithmetic<std::remove_cvref_t<decltype(std::declval<T>().getLower())>>;
    requires FloatingArithmetic<std::remove_cvref_t<decltype(std::declval<T>().getUpper())>>;
  };

  // A generic Vector3 concept that accepts both numerical and interval scalar components
  template<typename T>
  concept Vector3Like = requires (const T & v)
  {
    { v[0] } -> ScalarLike;
    { v[1] } -> ScalarLike;
    { v[2] } -> ScalarLike;
  };

  // A non-Interval, Float-only Vector3 concept
  template<typename V>
  concept FloatOnlyVector3Like = Vector3Like<V> && FloatingArithmetic<ScalarOf<V>>;

  // Determine common type of scalars
  template<typename A, typename B>
  using CommonTypeOfScalars = std::common_type_t<ScalarOf<A>, ScalarOf<B>>;

  // A multi-arg Interval-only Vec3 concept that requires both types be similar. Floating one follows
  template<typename A, typename B>
  concept IntervalOnlyVec3Common = Vector3Like<A> && Vector3Like<B> && requires { typename CommonTypeOfScalars<A, B>; } && std::convertible_to<ScalarOf<A>, CommonTypeOfScalars<A, B>> && std::convertible_to<ScalarOf<B>, CommonTypeOfScalars<A, B>> && IntervalScalarLike<CommonTypeOfScalars<A, B>>;

  template<typename A, typename B>
  concept FloatOnlyVec3Common = FloatOnlyVector3Like<A> && FloatOnlyVector3Like<B> && requires { typename CommonTypeOfScalars<A, B>; } && FloatingArithmetic<CommonTypeOfScalars<A, B>>;

  // Arity checker
  template<std::size_t N>
  concept Arity = (N == 2) || (N == 3) || (N == 4);

  // A concept reducing to the common type
  template<typename... Ts>
  concept FPCommon = std::floating_point<std::common_type_t<Ts...>>;

}
