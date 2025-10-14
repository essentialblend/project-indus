export module vector;

import std;
import concepts;
import interval;

export template<ScalarLike T, std::size_t N> requires Arity<N>
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

  template<FloatingArithmetic U> requires IntervalScalarLike<T>
  constexpr explicit Vector(const Vector<U, N>& value, const Vector<U, N>& error = Vector<U, N>{}) noexcept;

  template<FloatingArithmetic U> requires (!IntervalScalarLike<T>)
  constexpr explicit Vector(const Vector<Interval<U>, N>&) noexcept;

  constexpr Vector(const Vector&) noexcept = default;

  constexpr Vector(Vector&&) noexcept = default;

  constexpr Vector& operator=(const Vector&) noexcept = default;
  constexpr Vector& operator=(Vector&&) noexcept = default;

  constexpr auto operator<=>(const Vector&) noexcept = delete;
  constexpr bool operator==(const Vector&) const noexcept;

  // Vector / Point / Normal
  constexpr const T& operator[](std::size_t i) const & noexcept;
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

  template<FloatingArithmetic U> requires IntervalScalarLike<T>
  [[nodiscard]] constexpr Vector<U, N> getError() const noexcept;

  [[nodiscard]] constexpr bool isExact() const noexcept requires IntervalScalarLike<T>;
   
  ~Vector() = default;

private:
  std::array<T, N> m_elements{};
};

// Implementation remains in the interface file

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>::Vector(const T& val) noexcept
{
  m_elements.fill(val);
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>::Vector(const std::array<T, N>& values) noexcept : m_elements{ values } {}

template<ScalarLike T, std::size_t N> requires Arity<N>
template<typename ...Args> requires (sizeof...(Args) == N)
constexpr Vector<T, N>::Vector(Args... vals) noexcept : m_elements{ { static_cast<T>(vals)... } } {}

template<ScalarLike T, std::size_t N> requires Arity<N>
template<FloatingArithmetic U> requires IntervalScalarLike<T>
constexpr Vector<U, N> Vector<T, N>::getError() const noexcept
{
  Vector<U, N> out{};
  for (std::size_t i{}; i < N; ++i)
  {
    const U lo{ static_cast<U>(m_elements[i].getLower()) };
    const U hi{ static_cast<U>(m_elements[i].getUpper()) };

    out[i] = (hi - lo) * static_cast<U>(0.5);
  }

  return out;
}

template<ScalarLike T, std::size_t N> requires Arity<N>
template<FloatingArithmetic U> requires (!IntervalScalarLike<T>)
constexpr Vector<T, N>::Vector(const Vector<Interval<U>, N>& vI) noexcept
{
  for (std::size_t i{ 0 }; i < N; ++i)
  {
    (*this)[i] = T{ vI[i].getMid() };
  }
}

template<ScalarLike T, std::size_t N> requires Arity<N>
template<FloatingArithmetic U> requires IntervalScalarLike<T>
constexpr Vector<T, N>::Vector(const Vector<U, N>& value, const Vector<U, N>& error) noexcept
{
  using S = U;
  for (std::size_t i = 0; i < N; ++i)
  {
    m_elements[i] = T{ makeIntervalWithError<S>(value[i], error[i]) };
  }
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr bool Vector<T, N>::operator==(const Vector& other) const noexcept
{
  for (std::size_t i{}; i < N; ++i)
  {
    if (m_elements[i] != other.m_elements[i]) return false;
  }
  return true;
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr const T& Vector<T, N>::operator[](std::size_t i) const & noexcept
{
  return m_elements[i];
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr T& Vector<T, N>::operator[](std::size_t i) & noexcept
{
  return const_cast<T&>(std::as_const(*this)[i]);
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> Vector<T, N>::operator-() const noexcept
{
  Vector<T, N> result{};

  for (std::size_t i{}; i < N; ++i)
  {
    result.m_elements[i] = -m_elements[i];
  }
  return result;
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>& Vector<T, N>::operator+=(const Vector& rhs) noexcept
{
  for (std::size_t i{}; i < N; ++i)
    m_elements[i] += rhs.m_elements[i];
  return *this;
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>& Vector<T, N>::operator-=(const Vector& rhs) noexcept
{
  return (*this += -rhs);
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>& Vector<T, N>::operator*=(const T& s) noexcept
{
  for (std::size_t i{}; i < N; ++i) m_elements[i] *= s;
  return *this;
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N>& Vector<T, N>::operator/=(const T& s) noexcept
{
  const T inv = T{ 1 } / s;
  return (*this *= inv);
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr bool Vector<T, N>::isExact() const noexcept requires IntervalScalarLike<T>
{
  for (std::size_t i = 0; i < N; ++i)
    if (!m_elements[i].isDegenerate()) return false;
  return true;
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> Vector<T, N>::operator+(const Vector& other) const noexcept
{
  auto temp{ *this };
  temp += other;
  return temp;
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> Vector<T, N>::operator-(const Vector& other) const noexcept
{
  auto temp{ *this };
  temp -= other;
  return temp;
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> Vector<T, N>::operator*(const T& s) const noexcept
{
  auto temp{ *this };
  temp *= s;
  return temp;
}

template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> Vector<T, N>::operator/(const T& s) const noexcept
{
  auto temp{ *this };
  temp /= s;
  return temp;
}

// Symmetric free-operators
export template<ScalarLike T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> operator*(const T& s, const Vector<T, N>& v) noexcept 
{
  return v * s;
}