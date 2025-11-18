export module indus.core.math.trig.iii;

import indus.core.types;
import indus.core.concepts;

import indus.core.math.constants.i;
import indus.core.math.fp.ii;

export
{
  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T degreesToRadians(T deg) noexcept
  {
    return deg * (std::numbers::pi_v<T> / T{ 180 });
  }

  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T radiansToDegrees(T rad) noexcept
  {
    return rad * (T{ 180 } / std::numbers::pi_v<T>);
  }

  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T safeACos(T x) noexcept
  {
    return std::acos(clampUnit(x));
  }

  // Unused
  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T safeASin(T x) noexcept
  {
    const T xc{ clamp(x, T{ -1 }, T{ 1 }) };
    return std::asin(xc);
  }

  // Unused
  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T sinexOverx(T x) noexcept
  {
    const T ax{ std::abs(x) };

    if (ax < T{ 1e-3 })
    {
      // return T(1) - x2 / T(6) + sqr(x2) / T(120); 
      constexpr std::array<T, 3> c{ T{ 1 }, T{ -1 } / T{ 6 }, T{ 1 } / T{ 120 } };
      return evaluatePolynomial(sqr(x), c.begin(), c.end());
    }

    return std::sin(x) / x;
  }

  [[nodiscard]] Float cosineThetaLocal(const Vec3f& w) noexcept
  {
    return w[2];
  }

  [[nodiscard]] Float absCosineThetaLocal(const Vec3f& w) noexcept
  {
    return std::abs(w[2]);
  }

  [[nodiscard]] Float sineSqThetaLocal(const Vec3f& w) noexcept
  {
    return clamp(differenceOfProducts(Float{ 1 }, Float{ 1 }, w[2], w[2]), Float{}, Float{ 1 });
  }

  [[nodiscard]] Float sineThetaLocal(const Vec3f& w) noexcept
  {
    return safeSqrt(sineSqThetaLocal(w));
  }

  [[nodiscard]] Float cosineSqThetaLocal(const Vec3f& w) noexcept
  {
    return clamp(sqr(w[2]), Float{}, Float{ 1 });
  }

  [[nodiscard]] Float tanThetaLocal(const Vec3f& w) noexcept
  {
    const Float c{ absCosineThetaLocal(w) };

    return (c > Float{} ? (sineThetaLocal(w) / c) : infinity<Float>);
  }

  [[nodiscard]] Float cosPhiLocal(const Vec3f& w) noexcept
  {
    const Float s2{ sqr(w[0]) + sqr(w[1]) };

    if (s2 <= Float{}) return Float{ 1 };

    const Float inv{ Float{ 1 } / safeSqrt(s2) };

    return clamp(w[0] * inv, Float{ -1 }, Float{ 1 });
  }

  [[nodiscard]] Float sinPhiLocal(const Vec3f& w) noexcept
  {
    const Float s2{ sqr(w[0]) + sqr(w[1]) };

    if (s2 <= Float{}) return Float{};
    const Float inv{ Float{ 1 } / safeSqrt(s2) };

    return clamp(w[1] * inv, Float{ -1 }, Float{ 1 });
  }

  [[nodiscard]] Float cosDPhi(const Vec3f& a, const Vec3f& b) noexcept
  {
    const Float sa2{ sqr(a[0]) + sqr(a[1]) };
    const Float sb2{ sqr(b[0]) + sqr(b[1]) };

    if (sa2 <= Float{} || sb2 <= Float{}) return Float(1);

    const Float ia{ Float{ 1 } / safeSqrt(sa2) };
    const Float ib{ Float{ 1 } / safeSqrt(sb2) };

    return clamp(a[0] * ia * b[0] * ib + a[1] * ia * b[1] * ib, Float{ -1 }, Float{ 1 });
  }

  // Unused
  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T sphericalTheta(T z) noexcept
  {
    return safeACos(clamp(z, T{ -1 }, T{ 1 }));
  }

  // Unused
  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T sphericalPhi(T y, T x) noexcept
  {
    const T twoPi{ T(2) * std::numbers::pi_v<T> };

    return modPos(std::atan2(y, x), twoPi);
  }
}