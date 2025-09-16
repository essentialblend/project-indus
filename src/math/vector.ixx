export module vector;

import std;
import concepts;

export template<Arithmetic T, std::size_t N> requires Arity<N>
class Vector final
{
public:

  using value_type = T;
  static constexpr std::size_t arity = N;

  constexpr Vector() noexcept = default;

  [[nodiscard]] constexpr Vector(const T&) noexcept;
  [[nodiscard]] constexpr Vector(const std::array<T, N>&) noexcept;

  template<typename... Args> requires (sizeof...(Args) == N)
    constexpr Vector(Args...) noexcept;

  constexpr Vector(const Vector&) noexcept = default;
  constexpr Vector(Vector&&) noexcept = default;

  constexpr Vector& operator=(const Vector&) noexcept = default;
  constexpr Vector& operator=(Vector&&) noexcept = default;

  constexpr auto operator<=>(const Vector&) noexcept = delete;
  constexpr bool operator==(const Vector&) const noexcept;

  // Vector / Point / Normal
  constexpr const T& operator[](std::size_t i) const& noexcept;
  constexpr T& operator[](std::size_t i) & noexcept;

  constexpr Vector operator-() const noexcept;

  constexpr Vector operator+(const Vector&) const noexcept;
  constexpr Vector operator-(const Vector&) const noexcept;
  constexpr Vector operator*(const T&) const noexcept;
  constexpr Vector operator/(const T&) const noexcept;

  constexpr Vector& operator+=(const Vector&) noexcept;
  constexpr Vector& operator-=(const Vector&) noexcept;
  constexpr Vector& operator*=(const T&) noexcept;
  constexpr Vector& operator/=(const T&) noexcept;

  ~Vector() = default;

private:
  std::array<T, N> m_elements{};
};

// Implementation remains in the interface file

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>::Vector(const T& val) noexcept
{
  m_elements.fill(val);
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>::Vector(const std::array<T, N>& values) noexcept : m_elements{ values } {}

template<Arithmetic T, std::size_t N> requires Arity<N>
template<typename... Args> requires (sizeof...(Args) == N)
constexpr Vector<T, N>::Vector(Args... vals) noexcept : m_elements{ { static_cast<T>(vals)... } } {}


template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr const T& Vector<T, N>::operator[](std::size_t i) const & noexcept
{
  return m_elements[i];
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr T& Vector<T, N>::operator[](std::size_t i) & noexcept
{
  return const_cast<T&>(std::as_const(*this)[i]);
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr bool Vector<T, N>::operator==(const Vector& other) const noexcept
{
  for (std::size_t i{}; i < N; ++i)
  {
    if (m_elements[i] != other.m_elements[i]) return false;
  }
  return true;
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> Vector<T, N>::operator-() const noexcept
{
  Vector<T, N> result{};

  for (std::size_t i{}; i < N; ++i)
  {
    result.m_elements[i] = -m_elements[i];
  }
  return result;
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>& Vector<T, N>::operator+=(const Vector& rhs) noexcept
{
  for (std::size_t i{}; i < N; ++i)
    m_elements[i] += rhs.m_elements[i];
  return *this;
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>& Vector<T, N>::operator-=(const Vector& rhs) noexcept
{
  return (*this += -rhs);
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>& Vector<T, N>::operator*=(const T& s) noexcept
{
  for (std::size_t i{}; i < N; ++i) m_elements[i] *= s;
  return *this;
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>& Vector<T, N>::operator/=(const T& s) noexcept
{
  const T inv = T{ 1 } / s;
  return (*this *= inv);
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> Vector<T, N>::operator+(const Vector& other) const noexcept
{
  auto temp{ *this };
  temp += other;
  return temp;
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> Vector<T, N>::operator-(const Vector& other) const noexcept
{
  auto temp{ *this };
  temp -= other;
  return temp;
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> Vector<T, N>::operator*(const T& s) const noexcept
{
  auto temp{ *this };
  temp *= s;
  return temp;
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> Vector<T, N>::operator/(const T& s) const noexcept
{
  auto temp{ *this };
  temp /= s;
  return temp;
}

// Symmetric free-operators

export template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> operator*(const T& s, const Vector<T, N>& v) noexcept 
{
  return v * s;
}


// Free generic math-utils

export template<Arithmetic T, std::size_t N> requires Arity<N>
[[nodiscard]] constexpr T computeDot(const Vector<T, N>& a, const Vector<T, N>& b) noexcept
{
  T sum{};
  for (std::size_t i{}; i < N; ++i)
  {
    sum += a[i] * b[i];
  }

  return sum;
}

export template<Arithmetic T>
[[nodiscard]] constexpr Vector<T, 3> computeCross(const Vector<T, 3>& a, const Vector<T, 3>& b) noexcept
{
  return { (a[1] * b[2]) - (a[2] * b[1]), (a[2] * b[0]) - (a[0] * b[2]), (a[0] * b[1]) - (a[1] * b[0]) };
}

export template<Arithmetic T, std::size_t N> requires Arity<N>
[[nodiscard]] constexpr T euclideanLengthSq(const Vector<T, N>& v) noexcept
{
  T sum{};
  for (std::size_t i{}; i < N; ++i)
  {
    sum += v[i] * v[i];
  }

  return sum;
}

export template<Arithmetic T, std::size_t N> requires Arity<N>
[[nodiscard]] constexpr T euclideanLength(const Vector<T, N>& v)
{
  return std::sqrt(euclideanLengthSq(v));
}

// Changed to fix post math-arch restructure, needs attention
export template<Arithmetic T, std::size_t N> requires Arity<N>
[[nodiscard]] constexpr Vector<T, N> normalize(const Vector<T, N>& v)
{
  T lenSq = euclideanLengthSq(v);
  if (lenSq > T{ 0 }) {
    T invLen = T{ 1 } / std::sqrt(lenSq);
    return v * invLen;
  }
  // Return zero vector if degenerate
  return Vector<T, N>{ T{ 0 } };
}