export module point;

import std;
import concepts;
import vector;
import interval;

export template<ScalarLike T, std::size_t N> requires Arity234<N>
class Point final
{
public:
  using value_type = T;
  static constexpr std::size_t arity = N;

  constexpr Point() noexcept = default;
  explicit constexpr Point(const std::array<T, N>&) noexcept;

  template<typename... Args> requires (sizeof...(Args) == N)
    constexpr Point(Args...) noexcept;

  template<FloatingArithmetic U> requires IntervalScalarLike<T>
  constexpr explicit Point(const Point<U, N>& value, const Vector<U, N>& error = Vector<U, N>{}) noexcept;

  template<FloatingArithmetic U = T> requires (!IntervalScalarLike<T>)
  constexpr explicit Point(const Point<Interval<U>, N>&) noexcept;

  template<ScalarLike U> requires (!IntervalScalarLike<T> && !IntervalScalarLike<U>&& std::convertible_to<U, T>)
  constexpr explicit Point(const Point<U, N>&) noexcept;

  constexpr Point(const Point&) noexcept = default;
  constexpr Point(Point&&) noexcept = default;

  constexpr Point& operator=(const Point&) noexcept = default;
  constexpr Point& operator=(Point&&) noexcept = default;

  constexpr auto operator<=>(const Point&) noexcept = delete;
  constexpr bool operator==(const Point&) const noexcept;

  constexpr const T& operator[](std::size_t) const & noexcept;
  constexpr T& operator[](std::size_t) & noexcept;

  constexpr Point& operator*=(T x) noexcept;

  constexpr Point operator+(const Vector<T, N>&) const noexcept;
  constexpr Point operator-(const Vector<T, N>&) const noexcept;
  constexpr Point operator*(T x) const noexcept;
  constexpr Vector<T, N> operator-(const Point&) const noexcept;

  template<FloatingArithmetic U> requires IntervalScalarLike<T>
  [[nodiscard]] constexpr Vector<U, N> getError() const noexcept;

  [[nodiscard]] constexpr bool isExact() const noexcept requires IntervalScalarLike<T>;

  ~Point() = default;

private:
  std::array<T, N> m_elements{};
};

template<ScalarLike T, std::size_t N> requires Arity234<N>
constexpr Point<T, N>::Point(const std::array<T, N>& values) noexcept : m_elements{ values } {}

template<ScalarLike T, std::size_t N> requires Arity234<N>
template<typename ...Args> requires (sizeof...(Args) == N)
constexpr Point<T, N>::Point(Args... values) noexcept : m_elements{ { static_cast<T>(values)... } } {}

template<ScalarLike T, std::size_t N> requires Arity234<N>
template<ScalarLike U> requires (!IntervalScalarLike<T> && !IntervalScalarLike<U>&& std::convertible_to<U, T>)
constexpr Point<T, N>::Point(const Point<U, N>& other) noexcept 
{
  for (std::size_t i{}; i < N; ++i) 
  {
    m_elements[i] = static_cast<T>(other[i]);
  }
}


template<ScalarLike T, std::size_t N> requires Arity234<N>
template<FloatingArithmetic U> requires IntervalScalarLike<T>
constexpr Vector<U, N> Point<T, N>::getError() const noexcept
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

template<ScalarLike T, std::size_t N> requires Arity234<N>
constexpr bool Point<T, N>::isExact() const noexcept requires IntervalScalarLike<T>
{
  for (std::size_t i = 0; i < N; ++i)
    if (!m_elements[i].isDegenerate()) return false;
  
  return true;
}

template<ScalarLike T, std::size_t N> requires Arity234<N>
template<FloatingArithmetic U> requires IntervalScalarLike<T>
constexpr Point<T, N>::Point(const Point<U, N>& value, const Vector<U, N>& error) noexcept
{
  for (std::size_t i{}; i < N; ++i)
    m_elements[i] = makeIntervalWithError(value[i], error[i]);
}

template<ScalarLike T, std::size_t N> requires Arity234<N>
template<FloatingArithmetic U> requires (!IntervalScalarLike<T>)
constexpr Point<T, N>::Point(const Point<Interval<U>, N>& pI) noexcept
{
  for (std::size_t i{ 0 }; i < N; ++i)
  {
    (*this)[i] = T{ pI[i].getMid() };
  }
}

template<ScalarLike T, std::size_t N> requires Arity234<N>
constexpr bool Point<T, N>::operator==(const Point& p) const noexcept
{
  return m_elements == p.m_elements;
}

template<ScalarLike T, std::size_t N> requires Arity234<N>
constexpr const T& Point<T, N>::operator[](std::size_t i) const & noexcept
{
  return m_elements[i];
}

template<ScalarLike T, std::size_t N> requires Arity234<N>
constexpr T& Point<T, N>::operator[](std::size_t i) & noexcept
{
  return const_cast<T&>(std::as_const(*this)[i]);
}

template<ScalarLike T, std::size_t N> requires Arity234<N>
constexpr Point<T, N>& Point<T, N>::operator*=(T x) noexcept 
{
  for (std::size_t i{}; i < N; ++i) (*this)[i] = (*this)[i] * x;
  return *this;
}

template<ScalarLike T, std::size_t N> requires Arity234<N>
constexpr Point<T, N> Point<T, N>::operator+(const Vector<T, N>& v) const noexcept
{
  Point<T, N> result{};

  for (std::size_t i{}; i < N; ++i)
  {
    result.m_elements[i] = m_elements[i] + v[i];
  }
  return result;
}

template<ScalarLike T, std::size_t N> requires Arity234<N>
constexpr Point<T, N> Point<T, N>::operator-(const Vector<T, N>& v) const noexcept
{
  return (*this) + (-v);
}

template<ScalarLike T, std::size_t N> requires Arity234<N>
constexpr Vector<T, N> Point<T, N>::operator-(const Point<T, N>& p) const noexcept
{
  Vector<T, N> result{};
  for (std::size_t i = 0; i < N; ++i)
  {
    result[i] = m_elements[i] - p.m_elements[i];
  }

  return result;
}

template<ScalarLike T, std::size_t N> requires Arity234<N>
constexpr Point<T, N> Point<T, N>::operator*(T x) const noexcept
{
  Point<T, N> p{ *this };
  p *= x;
  return p;
}

// Free

template<ScalarLike T, std::size_t N> requires Arity234<N>
constexpr Point<T, N> operator*(T x, Point<T, N> p) noexcept 
{
  p *= x;
  return p;
}