export module concepts;

import std;

export template<typename T>
concept Arithmetic = std::is_arithmetic_v<std::remove_cvref_t<T>>;

export template<typename T>
concept FloatingArithmetic = std::is_floating_point_v<T>;

export template<typename T>
concept IntegralArithmetic = std::is_integral_v<T>;

export template<typename T>
concept VectorLike = requires (const T & v)
{
  { v[0] } -> Arithmetic;
  { v[1] } -> Arithmetic;
  { v[2] } -> Arithmetic;
};

// Restrict vector to 2, 3, and 4 element containers
export template<std::size_t N>
concept Arity = (N == 2 || N == 3 || N == 4);