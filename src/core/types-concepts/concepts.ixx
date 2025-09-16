export module concepts;

import std;

export template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

export template<typename T>
concept FloatingArithmetic = std::is_floating_point_v<T>;

export template<typename T>
concept IntegralArithmetic = std::is_integral_v<T>;


// Restrict vector to 2, 3, and 4 element containers
export template<std::size_t N>
concept Arity = (N == 2 || N == 3 || N == 4);