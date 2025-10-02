export module efloat;

import std;
import concepts;
import mathconstants;

// Pending deeper understanding
export template<FloatingArithmetic T>
class EFloat final
{
public:
  constexpr EFloat() noexcept;
  constexpr explicit EFloat(T v) noexcept;
  constexpr EFloat(T v, T err) noexcept;
  constexpr EFloat(T v, T low, T high) noexcept;

  [[nodiscard]] constexpr T getValue() const noexcept;
  [[nodiscard]] constexpr T getLowerBound() const noexcept;
  [[nodiscard]] constexpr T getUpperBound() const noexcept;
  [[nodiscard]] constexpr T getAbsError() const noexcept;
  
  explicit constexpr operator T() const noexcept;

private:
  T m_v{};
  T m_low{};
  T m_high{};
};

template<FloatingArithmetic T>
constexpr EFloat<T>::EFloat() noexcept : m_v{}, m_low{ m_v }, m_high{ m_v } {}

template<FloatingArithmetic T>
constexpr EFloat<T>::EFloat(T v) noexcept : m_v{ v }, m_low{ nextFloatDown(v) }, m_high{ nextFloatUp(v) } {}

template<FloatingArithmetic T>
constexpr EFloat<T>::EFloat(T v, T err) noexcept : m_v{ v }, m_low{ nextFloatDown(v - std::abs(err)) }, m_high{ nextFloatUp(v + std::abs(err)) } {}

template<FloatingArithmetic T>
constexpr EFloat<T>::EFloat(T v, T lo, T hi) noexcept : m_v{ v }, m_low{ std::min(lo,hi) }, m_high{ std::max(lo,hi) } {}

template<FloatingArithmetic T> constexpr T EFloat<T>::getValue() const noexcept 
{ 
  return m_v; 
}

template<FloatingArithmetic T> constexpr T EFloat<T>::getLowerBound() const noexcept 
{ 
  return m_low;
}

template<FloatingArithmetic T> constexpr T EFloat<T>::getUpperBound() const noexcept 
{ 
  return m_high;
}

template<FloatingArithmetic T> constexpr T EFloat<T>::getAbsError() const noexcept 
{ 
  return std::max(m_v - m_low, m_high - m_v);
}

template<FloatingArithmetic T> constexpr EFloat<T>::operator T() const noexcept 
{ 
  return m_v; 
}

// Free

export
{
  template<FloatingArithmetic T>
  EFloat<T> makeEFloat(T v, T err) noexcept
  {
    return EFloat<T>{v, err};
  }

  template<FloatingArithmetic T>
  EFloat<T> operator+(const EFloat<T>& a, const EFloat<T>& b) noexcept
  {
    const T v{ a.Value() + b.Value() };
    const T low{ addRoundDown(a.getLowerBound(), b.getLowerBound()) };
    const T high{ addRoundUp(a.getUpperBound(), b.getUpperBound()) };

    return EFloat<T>{v, low, high};
  }

  template<FloatingArithmetic T>
  EFloat<T> operator-(const EFloat<T>& a, const EFloat<T>& b) noexcept
  {
    const T v{ a.Value() - b.Value() };
    const T low{ subRoundDown(a.getLowerBound(), b.getUpperBound()) };
    const T high{ subRoundUp(a.getUpperBound(), b.getLowerBound()) };

    return EFloat<T>{v, low, high};
  }

  template<FloatingArithmetic T>
  EFloat<T> operator*(const EFloat<T>& a, const EFloat<T>& b) noexcept
  {
    const T v{ a.getValue() * b.getValue() };
    const T ll{ mulRoundDown(a.getLowerBound(), b.getLowerBound()) };
    const T lh{ mulRoundDown(a.getLowerBound(), b.getUpperBound()) };
    const T hl{ mulRoundDown(a.getUpperBound(), b.getLowerBound()) };
    const T hh{ mulRoundDown(a.getUpperBound(), b.getUpperBound()) };
    const T lo{ std::min(std::min(ll, lh), std::min(hl, hh)) };
    const T LL{ mulRoundUp(a.getLowerBound(), b.getLowerBound()) };
    const T LH{ mulRoundUp(a.getLowerBound(), b.getUpperBound()) };
    const T HL{ mulRoundUp(a.getUpperBound(), b.getLowerBound()) };
    const T HH{ mulRoundUp(a.getUpperBound(), b.getUpperBound()) };
    const T hi{ std::max(std::max(LL, LH), std::max(HL, HH)) };

    return EFloat<T>{v, lo, hi};
  }

  template<FloatingArithmetic T>
  EFloat<T> operator/(const EFloat<T>& a, const EFloat<T>& b) noexcept
  {
    const T v{ a.Value() / b.Value() };

    if (b.getLowerBound() <= T(0) && b.getUpperBound() >= T(0)) return EFloat<T>{v, -infinity<T>, +infinity<T>};

    const T rlo{ divRoundDown(T(1), b.getUpperBound()) };
    const T rhi{ divRoundUp(T(1), b.getLowerBound()) };

    const EFloat<T> recip{ a.Value() * T(0) + T(1), std::min(rlo, rhi), std::max(rlo, rhi) };

    return a * recip;
  }

  template<FloatingArithmetic T>
  EFloat<T> fmaE(const EFloat<T>& x, const EFloat<T>& y, const EFloat<T>& z) noexcept
  {
    const T v{ std::fma(x.getValue(), y.getValue(), z.getValue()) };

    const T pmin{ std::min(std::min(mulRoundDown(x.getLowerBound(), y.getLowerBound()), mulRoundDown(x.getLowerBound(), y.getUpperBound())), std::min(mulRoundDown(x.getUpperBound(), y.getLowerBound()), mulRoundDown(x.getUpperBound(), y.getUpperBound()))) };

    const T pmax{ std::max(std::max(mulRoundUp(x.getLowerBound(), y.getLowerBound()), mulRoundUp(x.getLowerBound(), y.getUpperBound())), std::max(mulRoundUp(x.getUpperBound(), y.getLowerBound()), mulRoundUp(x.getUpperBound(), y.getUpperBound()))) };

    const T lo{ addRoundDown(pmin, z.getLowerBound()) };
    const T hi{ addRoundUp(pmax, z.getUpperBound()) };

    return EFloat<T>{v, lo, hi};
  }

  template<FloatingArithmetic T>
  EFloat<T> sqrtE(const EFloat<T>& x) noexcept
  {
    const T v{ std::sqrt(std::max(T(0), x.getValue())) };
    const T lo{ sqrtRoundDown(std::max(T(0), x.getLowerBound())) };
    const T hi{ sqrtRoundUp(std::max(T(0), x.getUpperBound())) };

    return EFloat<T>{v, lo, hi};
  }

  template<FloatingArithmetic T>
  [[nodiscard]] std::optional<std::pair<EFloat<T>, EFloat<T>>> evaluateQuadraticE(const EFloat<T>& a, const EFloat<T>& b, const EFloat<T>& c) noexcept
  {
    const bool aZero{ (a.LowerBound() == T(0) && a.UpperBound() == T(0)) };

    if (aZero) 
    {
      const bool bZero{ (b.LowerBound() == T(0) && b.UpperBound() == T(0)) };
      
      if (bZero) return std::nullopt;
      
      const EFloat<T> t{ -c / b };
      
      return std::make_pair(t, t);
    }

    const EFloat<T> disc = b * b - EFloat<T>(T(4)) * a * c;

    if (disc.UpperBound() < T(0)) return std::nullopt;

    const EFloat<T> sdisc{ sqrtE(disc) };
    const bool bNeg{ (b.Value() < T(0)) };
    const EFloat<T> qb{ EFloat<T>(-T(0.5)) * (bNeg ? (b - sdisc) : (b + sdisc)) };

    const bool qZero{ (qb.LowerBound() <= T(0) && qb.UpperBound() >= T(0)) };
    
    if (qZero) 
    {
      const EFloat<T> t{ -c / b };
      return std::make_pair(t, t);
    }

    EFloat<T> t0{ qb / a };
    EFloat<T> t1{ c / qb };

    if (t1.Value() < t0.Value()) std::swap(t0, t1);

    return std::make_pair(t0, t1);
  }
}