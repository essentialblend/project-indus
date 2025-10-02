export module interval;

import std;
import mathconstants;
import concepts;
import types;

// Pending deeper understanding
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

private:
  T m_low{}, m_high{};
};

export using Intervalf = Interval<Float>;

template<FloatingArithmetic T>
constexpr Interval<T>::Interval() noexcept : m_low{ +infinity<T> }, m_high{ -infinity<T> } {}

template<FloatingArithmetic T>
constexpr Interval<T>::Interval(T v) noexcept : m_low{ v }, m_high{ v } {}

template<FloatingArithmetic T>
constexpr Interval<T>::Interval(T low, T high) noexcept : m_low{ low < high ? low : high }, m_high{ low < high ? high : low } {}

template<FloatingArithmetic T>
constexpr T Interval<T>::getLower() const noexcept 
{ 
  return m_low; 
}

template<FloatingArithmetic T>
constexpr T Interval<T>::getUpper() const noexcept 
{ 
  return m_high; 
}

template<FloatingArithmetic T>
constexpr T Interval<T>::getMid() const noexcept 
{ 
  return (m_low + m_high) * T(0.5); 
}

template<FloatingArithmetic T>
constexpr T Interval<T>::getWidth() const noexcept 
{ 
  return m_high - m_low; 
}

template<FloatingArithmetic T>
constexpr bool Interval<T>::contains(T x) const noexcept 
{ 
  return (m_low <= x) && (x <= m_high); 
}

template<FloatingArithmetic T>
constexpr bool Interval<T>::isDegenerate() const noexcept 
{ 
  return m_low == m_high; 
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
  Interval<T> addI(const Interval<T>& a, const Interval<T>& b) noexcept
  {
    return Interval<T>{ addRoundDown(a.getLower(), b.getLower()), addRoundUp(a.getUpper(), b.getUpper()) };
  }

  template<FloatingArithmetic T>
  Interval<T> subI(const Interval<T>& a, const Interval<T>& b) noexcept
  {
    return Interval<T>{ subRoundDown(a.getLower(), b.getUpper()), subRoundUp(a.getUpper(), b.getLower()) };
  }

  template<FloatingArithmetic T>
  Interval<T> mulI(const Interval<T>& a, const Interval<T>& b) noexcept
  {
    const T ll{ mulRoundDown(a.getLower(), b.getLower()) };
    const T lh{ mulRoundDown(a.getLower(), b.getUpper()) };
    const T hl{ mulRoundDown(a.getUpper(), b.getLower()) };
    const T hh{ mulRoundDown(a.getUpper(), b.getUpper()) };
    const T lo{ std::min(std::min(ll, lh), std::min(hl, hh)) };

    const T LL{ mulRoundUp(a.getLower(), b.getLower()) };
    const T LH{ mulRoundUp(a.getLower(), b.getUpper()) };
    const T HL{ mulRoundUp(a.getUpper(), b.getLower()) };
    const T HH{ mulRoundUp(a.getUpper(), b.getUpper()) };
    const T hi{ std::max(std::max(LL, LH), std::max(HL, HH)) };

    return Interval<T>{ lo, hi };
  }

  template<FloatingArithmetic T>
  Interval<T> divI(const Interval<T>& a, const Interval<T>& b) noexcept
  {
    if (b.getLower() <= T(0) && b.getUpper() >= T(0)) return fullInterval<T>();

    const T rlo{ divRoundDown(T(1), b.getUpper()) };
    const T rhi{ divRoundUp(T(1), b.getLower()) };

    return mulI(a, Interval<T>{ rlo, rhi });
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
    const T pmin{ std::min(std::min(mulRoundDown(x.getLower(), y.getLower()), mulRoundDown(x.getLower(), y.getUpper())), std::min(mulRoundDown(x.getUpper(), y.getLower()), mulRoundDown(x.getUpper(), y.getUpper()))) };

    const T pmax{ std::max(std::max(mulRoundUp(x.getLower(), y.getLower()), mulRoundUp(x.getLower(), y.getUpper())), std::max(mulRoundUp(x.getUpper(), y.getLower()), mulRoundUp(x.getUpper(), y.getUpper()))) };

    return Interval<T>{ addRoundDown(pmin, z.getLower()), addRoundUp(pmax, z.getUpper()) };
  }
}