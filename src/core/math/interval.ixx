export module indus.core.math.interval;

import indus.core.concepts;

import indus.core.math.constants.i;
import indus.core.math.fp.ii;

export template<FloatingArithmetic T>
class Interval final 
{
public:
  constexpr Interval() noexcept;
  constexpr explicit Interval(T v) noexcept;
  constexpr Interval(T low, T high) noexcept;

  [[nodiscard]] constexpr T getLower() const noexcept;
  [[nodiscard]] constexpr T getUpper() const noexcept;
  [[nodiscard]] constexpr T getMid()   const noexcept;
  [[nodiscard]] constexpr T getWidth() const noexcept;
  [[nodiscard]] constexpr bool contains(T x) const noexcept;
  [[nodiscard]] constexpr bool isDegenerate() const noexcept;
  [[nodiscard]] constexpr bool isEmpty() const noexcept;
  
  [[nodiscard]] constexpr Interval operator-() const noexcept;

  constexpr Interval& operator+=(const Interval& other) noexcept;
  constexpr Interval& operator-=(const Interval& other) noexcept;
  constexpr Interval& operator*=(const Interval& other) noexcept;
  constexpr Interval& operator/=(const Interval& other) noexcept;

  [[nodiscard]] constexpr Interval operator+(const Interval& other) const noexcept;
  [[nodiscard]] constexpr Interval operator-(const Interval& other) const noexcept;
  [[nodiscard]] constexpr Interval operator*(const Interval& other) const noexcept;
  [[nodiscard]] constexpr Interval operator/(const Interval& other) const noexcept;


private:
  T m_low{};
  T m_upper{};
};

template<FloatingArithmetic T>
constexpr Interval<T>::Interval() noexcept : m_low{ +infinity<T> }, m_upper{ -infinity<T> } {}

template<FloatingArithmetic T>
constexpr Interval<T>::Interval(T v) noexcept : m_low{ v }, m_upper{ v } {}

template<FloatingArithmetic T>
constexpr Interval<T>::Interval(T low, T high) noexcept : m_low{ low < high ? low : high }, m_upper{ low < high ? high : low } {}

template<FloatingArithmetic T>
constexpr T Interval<T>::getLower() const noexcept 
{ 
  return m_low; 
}

template<FloatingArithmetic T>
constexpr T Interval<T>::getUpper() const noexcept 
{ 
  return m_upper; 
}

template<FloatingArithmetic T>
constexpr T Interval<T>::getMid() const noexcept 
{ 
  return (m_low + m_upper) * T{ 0.5 };
}

template<FloatingArithmetic T>
constexpr T Interval<T>::getWidth() const noexcept 
{ 
  return m_upper - m_low; 
}

template<FloatingArithmetic T>
constexpr bool Interval<T>::contains(T x) const noexcept 
{ 
  return (m_low <= x) && (x <= m_upper); 
}

template<FloatingArithmetic T>
constexpr bool Interval<T>::isDegenerate() const noexcept 
{ 
  return m_low == m_upper; 
}

template<FloatingArithmetic T>
constexpr bool Interval<T>::isEmpty() const noexcept
{
  return m_low > m_upper;
}

template<FloatingArithmetic T>
constexpr Interval<T> Interval<T>::operator-() const noexcept 
{
  if (isEmpty()) return *this;
  return Interval<T>{ -m_upper, -m_low };
}

template<FloatingArithmetic T>
constexpr Interval<T>& Interval<T>::operator+=(const Interval& other) noexcept
{
  m_low = addRoundDown(m_low, other.m_low);
  m_upper = addRoundUp(m_upper, other.m_upper);

  return *this;
}

template<FloatingArithmetic T>
constexpr Interval<T>& Interval<T>::operator-=(const Interval& other) noexcept
{
  m_low = subRoundDown(m_low, other.m_upper); 
  m_upper = subRoundUp(m_upper, other.m_low);
  
  return *this;
}

template<FloatingArithmetic T>
constexpr Interval<T>& Interval<T>::operator*=(const Interval& other) noexcept
{
  const T ll{ mulRoundDown(m_low,  other.m_low) };
  const T lh{ mulRoundDown(m_low,  other.m_upper) };
  const T hl{ mulRoundDown(m_upper, other.m_low) };
  const T hh{ mulRoundDown(m_upper, other.m_upper) };
  
  const T LL{ mulRoundUp(m_low,  other.m_low) };
  const T LH{ mulRoundUp(m_low,  other.m_upper) };
  const T HL{ mulRoundUp(m_upper, other.m_low) };
  const T HH{ mulRoundUp(m_upper, other.m_upper) };
 
  m_low = std::min(std::min(ll, lh), std::min(hl, hh));
  m_upper = std::max(std::max(LL, LH), std::max(HL, HH));

  return *this;
}

template<FloatingArithmetic T>
constexpr Interval<T>& Interval<T>::operator/=(const Interval& other) noexcept
{
  if (other.getLower() <= T{} && other.getUpper() >= T{})
  {
    m_low = -infinity<T>;
    m_upper = +infinity<T>;
    
    return *this;
  }

  const T rlo{ divRoundDown(T{ 1 }, other.getUpper()) };
  const T rhi{ divRoundUp(T{ 1 }, other.getLower()) };
  
  (*this) *= { std::min(rlo, rhi), std::max(rlo, rhi) };

  return *this;
}

template<FloatingArithmetic T>
constexpr Interval<T> Interval<T>::operator+(const Interval& other) const noexcept
{
  Interval<T> r{ *this };
  r += other;

  return r;
}

template<FloatingArithmetic T>
constexpr Interval<T> Interval<T>::operator-(const Interval& other) const noexcept
{
  Interval<T> r{ *this };
  r -= other;

  return r;
}

template<FloatingArithmetic T>
constexpr Interval<T> Interval<T>::operator*(const Interval& other) const noexcept
{
  Interval<T> r{ *this };
  r *= other;

  return r;
}

template<FloatingArithmetic T>
constexpr Interval<T> Interval<T>::operator/(const Interval& other) const noexcept
{
  Interval<T> r{ *this };
  r /= other;

  return r;
}

// Free

export
{
  template<FloatingArithmetic T>
  Interval<T> makeInterval(T v) noexcept
  {
    return Interval<T>{ nextFloatDown(v), nextFloatUp(v) };
  }

  template<FloatingArithmetic T>
  Interval<T> makeIntervalWithError(T v, T err) noexcept
  {
    const T l{ nextFloatDown(v - std::abs(err)) };
    const T u{ nextFloatUp(v + std::abs(err)) };

    return Interval<T>{ l, u };
  }

  template<FloatingArithmetic T>
  Interval<T> fullInterval() noexcept
  {
    return Interval<T>{ -infinity<T>, +infinity<T> };
  }

  template<FloatingArithmetic T>
  Interval<T> sqrtI(const Interval<T>& x) noexcept
  {
    const T l{ std::max(T(0), x.getLower()) };
    const T u{ std::max(T(0), x.getUpper()) };

    return Interval<T>{ sqrtRoundDown(l), sqrtRoundUp(u) };
  }

  template<FloatingArithmetic T>
  Interval<T> fmaI(const Interval<T>& x, const Interval<T>& y, const Interval<T>& z) noexcept
  {
    const T xl{ x.getLower() }; const T xh{ x.getUpper() };
    const T yl{ y.getLower() }; const T yh{ y.getUpper() };
    const T zl{ z.getLower() }; const T zh{ z.getUpper() };

    const T lo{ std::min({ FMARoundDown(xl,yl,zl), FMARoundDown(xl,yh,zl), FMARoundDown(xh,yl,zl), FMARoundDown(xh,yh,zl) }) };

    const T hi{ std::max({ FMARoundUp(xl,yl,zh), FMARoundUp(xl,yh,zh), FMARoundUp(xh,yl,zh), FMARoundUp(xh,yh,zh) }) };

    return Interval<T>{lo, hi};
  }
};