export module point;

import std;
import concepts;
import vector;

export template<Arithmetic T, std::size_t N> requires Arity<N>
class Point final
{
public:
  using value_type = T;
  static constexpr std::size_t arity = N;

  constexpr Point() noexcept = default;
  explicit constexpr Point(const std::array<T, N>&) noexcept;

  template<typename... Args> requires (sizeof...(Args) == N)
    constexpr Point(Args...) noexcept;

  constexpr Point(const Point&) noexcept = default;
  constexpr Point(Point&&) noexcept = default;

  constexpr Point& operator=(const Point&) noexcept = default;
  constexpr Point& operator=(Point&&) noexcept = default;

  constexpr auto operator<=>(const Point&) noexcept = delete;
  constexpr bool operator==(const Point&) const noexcept;

  constexpr const T& operator[](std::size_t) const & noexcept;
  constexpr T& operator[](std::size_t) & noexcept;

  constexpr Point operator+(const Vector<T, N>&) const noexcept;
  constexpr Point operator-(const Vector<T, N>&) const noexcept;
  constexpr Vector<T, N> operator-(const Point&) const noexcept;

  ~Point() = default;

private:
  std::array<T, N> m_elements{};
};

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Point<T, N>::Point(const std::array<T, N>& values) noexcept : m_elements{ values } {}

template<Arithmetic T, std::size_t N> requires Arity<N>
template<typename ...Args> requires (sizeof...(Args) == N)
constexpr Point<T, N>::Point(Args... values) noexcept : m_elements{ { static_cast<T>(values)... } } {}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr bool Point<T, N>::operator==(const Point& p) const noexcept
{
  return m_elements == p.m_elements;
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr const T& Point<T, N>::operator[](std::size_t i) const & noexcept
{
  return m_elements[i];
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr T& Point<T, N>::operator[](std::size_t i) & noexcept
{
  return const_cast<T&>(std::as_const(*this)[i]);
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Point<T, N> Point<T, N>::operator+(const Vector<T, N>& v) const noexcept
{
  Point<T, N> result{};

  for (std::size_t i{}; i < N; ++i)
  {
    result.m_elements[i] = m_elements[i] + v[i];
  }
  return result;
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Point<T, N> Point<T, N>::operator-(const Vector<T, N>& v) const noexcept
{
  return (*this) + (-v);
}

template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> Point<T, N>::operator-(const Point<T, N>& p) const noexcept
{
  Vector<T, N> result{};
  for (std::size_t i = 0; i < N; ++i)
  {
    result[i] = m_elements[i] - p.m_elements[i];
  }

  return result;
}