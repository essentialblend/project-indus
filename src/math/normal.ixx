export module normal;

import concepts;
import vector;

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

export template<Arithmetic T, std::size_t N> requires Arity<N>
constexpr Vector<T, N> operator+(const Vector<T, N>& v, const Normal<T>& n) noexcept
{
  return v + Vector<T, N>{ n[0], n[1], n[2] };
}


// Free
export template<Arithmetic T>
T computeDot(const Normal<T>& n, const Vector<T, 3>& v) 
{
  return n[0] * v[0] + n[1] * v[1] + n[2] * v[2];
}

export template<Arithmetic T>
T computeDot(const Vector<T, 3>& v, const Normal<T>& n) 
{
  return v[0] * n[0] + v[1] * n[1] + v[2] * n[2];
}

export template<Arithmetic T>
T computeDot(const Normal<T>& first, const Normal<T>& second) 
{
  return first[0] * second[0] + first[1] * second[1] + first[2] * second[2];
}

export template<Arithmetic T>
T computeAbsDot(const Normal<T>& n, const Vector<T, 3>& v) 
{
  return std::abs(computeDot(n, v));
}

export template<Arithmetic T>
Normal<T> normalize(const Normal<T>& n)
{
  T lenSq = n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
  if (lenSq > T{ 0 }) {
    T invLen = T{ 1 } / std::sqrt(lenSq);
    return n * invLen;
  }
  // Degenerate input: return zero normal
  return Normal<T>{ T{ 0 }, T{ 0 }, T{ 0 } };
}

export template<Arithmetic T>
[[nodiscard]] constexpr Vector<T, 3> computeCross(const Normal<T>& a, const Vector<T, 3>& b) noexcept
{
  return { (a[1] * b[2]) - (a[2] * b[1]), (a[2] * b[0]) - (a[0] * b[2]), (a[0] * b[1]) - (a[1] * b[0]) };
}

