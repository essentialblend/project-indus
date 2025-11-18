export module indus.core.geom.normal;

import indus.core.concepts;
import indus.core.geom.vector;

export template<Arithmetic T>
class Normal final 
{
public:
  using value_type = T;
  static constexpr std::size_t arity = 3;

  constexpr Normal() noexcept = default;

  [[nodiscard]] constexpr Normal(const T&) noexcept;
  [[nodiscard]] constexpr Normal(const std::array<T, 3>&) noexcept;
  explicit constexpr Normal(const Vector<T, 3>&) noexcept;
  constexpr Normal(const T&, const T&, const T&) noexcept;

  constexpr Normal(const Normal&) noexcept = default;
  constexpr Normal(Normal&&) noexcept = default;

  constexpr Normal& operator=(const Normal&) noexcept = default;
  constexpr Normal& operator=(Normal&&) noexcept = default;

  constexpr auto operator<=>(const Normal&) noexcept = delete;
  constexpr bool operator==(const Normal&) const noexcept;

  constexpr const T& operator[](std::size_t) const & noexcept;
  constexpr T& operator[](std::size_t) & noexcept;

  constexpr Normal operator-() const noexcept;

  constexpr Normal operator+(const Normal&) const noexcept;
  constexpr Normal operator-(const Normal&) const noexcept;
  constexpr Normal operator*(const T&) const noexcept;
  constexpr Normal operator/(const T&) const noexcept;

  constexpr Normal& operator+=(const Normal&) noexcept;
  constexpr Normal& operator-=(const Normal&) noexcept;
  constexpr Normal& operator*=(const T&) noexcept;
  constexpr Normal& operator/=(const T&) noexcept;

  ~Normal() = default;

private:
  std::array<T, arity> m_elements{};
};

// Impl

template<Arithmetic T>
constexpr Normal<T>::Normal(const T& v) noexcept : m_elements{ v, v, v } {}

template<Arithmetic T>
constexpr Normal<T>::Normal(const std::array<T, 3>& arr) noexcept : m_elements{ arr } {}

template<Arithmetic T>
constexpr Normal<T>::Normal(const T& x, const T& y, const T& z) noexcept : m_elements{ x, y, z } {}

template<Arithmetic T>
constexpr Normal<T>::Normal(const Vector<T, 3>& v) noexcept : m_elements{ v[0], v[1], v[2] } {}

template<Arithmetic T>
constexpr bool Normal<T>::operator==(const Normal& other) const noexcept 
{
  return m_elements == other.m_elements;
}

template<Arithmetic T>
constexpr const T& Normal<T>::operator[](std::size_t i) const& noexcept 
{ 
  return m_elements[i]; 
}

template<Arithmetic T>
constexpr T& Normal<T>::operator[](std::size_t i) & noexcept
{
  return const_cast<T&>(std::as_const(*this)[i]);
}

template<Arithmetic T>
constexpr Normal<T> Normal<T>::operator-() const noexcept 
{
  return Normal{ -m_elements[0], -m_elements[1], -m_elements[2] };
}

template<Arithmetic T>
constexpr Normal<T> Normal<T>::operator+(const Normal& rhs) const noexcept 
{
  Normal tmp{ *this };
  tmp += rhs;
  return tmp;
}

template<Arithmetic T>
constexpr Normal<T> Normal<T>::operator-(const Normal& rhs) const noexcept 
{
  Normal tmp{ *this };
  tmp -= rhs;
  return tmp;
}

template<Arithmetic T>
constexpr Normal<T> Normal<T>::operator*(const T& s) const noexcept 
{
  Normal tmp{ *this };
  tmp *= s;
  return tmp;
}

template<Arithmetic T>
constexpr Normal<T> Normal<T>::operator/(const T& s) const noexcept 
{
  Normal tmp{ *this };
  tmp /= s;
  return tmp;
}

template<Arithmetic T>
constexpr Normal<T>& Normal<T>::operator+=(const Normal& rhs) noexcept 
{
  m_elements[0] += rhs.m_elements[0];
  m_elements[1] += rhs.m_elements[1];
  m_elements[2] += rhs.m_elements[2];
  return *this;
}

template<Arithmetic T>
constexpr Normal<T>& Normal<T>::operator-=(const Normal& rhs) noexcept 
{
  m_elements[0] -= rhs.m_elements[0];
  m_elements[1] -= rhs.m_elements[1];
  m_elements[2] -= rhs.m_elements[2];
  return *this;
}

template<Arithmetic T>
constexpr Normal<T>& Normal<T>::operator*=(const T& s) noexcept 
{
  m_elements[0] *= s;
  m_elements[1] *= s;
  m_elements[2] *= s;
  return *this;
}

template<Arithmetic T>
constexpr Normal<T>& Normal<T>::operator/=(const T& s) noexcept 
{
  m_elements[0] /= s;
  m_elements[1] /= s;
  m_elements[2] /= s;
  return *this;
}

export template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr Vector<T, N> operator+(const Vector<T, N>& v, const Normal<T>& n) noexcept
{
  return v + Vector<T, N>{ n[0], n[1], n[2] };
}
