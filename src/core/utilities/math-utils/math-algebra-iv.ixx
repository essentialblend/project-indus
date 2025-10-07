export module mathalgebra;

import std;
import types;
import concepts;
import mathfp;

export
{
  // Unused
  [[nodiscard]] Float computeCross2(const Vec2f& x, const Vec2f& y) noexcept
  {
    return differenceOfProducts(x[0], y[1], x[1], y[0]);
  }

  template<FloatOnlyVector3Like T>
  [[nodiscard]] T normalizeSafe(const T& v, ScalarOf<T> eps) noexcept
  {
    if (!isFinite(v)) return T{};

    const auto len2{ sumOfProducts(v[0], v[0], v[1], v[1], v[2], v[2]) };
  
    if (!isFinite(len2) || !(len2 > sqr(eps))) return T{};

    const auto inv{ static_cast<decltype(len2)>(1) / std::sqrt(len2) };

    // return T{ v[0] * inv, v[1] * inv, v[2] * inv };
    return T{ v * inv };
  }

  [[nodiscard]] std::pair<Vec3f, Float> normalizeSafeWithLen(const Vec3f& v, Float eps) noexcept
  {
    if (!isFinite(v)) return { Vec3f{}, Float{} };

    const Float lenSq{ sumOfProducts(v[0], v[0], v[1], v[1], v[2], v[2]) };

    if (lenSq <= sqr(eps)) return { Vec3f{}, Float{} };

    const Float len{ std::sqrt(lenSq) };

    const Float inv{ Float(1) / len };

    return { Vec3f{ v * inv }, len };
  }

  [[nodiscard]] bool isNormalizedWithin(const Vec3f& v, Float tol) noexcept
  {
    if (!isFinite(v)) return false;

    const Float len2{ sumOfProducts(v[0], v[0], v[1], v[1], v[2], v[2]) };

    return std::abs(len2 - Float{ 1 }) <= (Float{ 2 } * tol + tol * tol);
  }

  template<FloatOnlyVector3Like V>
  [[nodiscard]] constexpr auto euclideanLengthSq(const V& v) noexcept
  { 
    return sumOfProducts(v[0], v[0], v[1], v[1], v[2], v[2]);
  }

  template<FloatOnlyVector3Like V>
  [[nodiscard]] constexpr auto euclideanLength(const V& v) noexcept
  {
    return std::sqrt(euclideanLengthSq(v));
  }

  template<typename M, typename N> requires FloatOnlyVec3Common<M, N>
  [[nodiscard]] constexpr auto computeDot(const M& a, const N& b) noexcept
  {
    using S = CommonTypeOfScalars<M, N>;
    return S{ sumOfProducts(S{ a[0] }, S{ b[0] }, S{ a[1] }, S{ b[1] }, S{ a[2] }, S{ b[2] }) };
  }

  template<typename M, typename N> requires IntervalOnlyVec3Common<M, N>
  [[nodiscard]] constexpr auto computeDot(const M& a, const N& b) noexcept
  {
    using S = CommonTypeOfScalars<M, N>;

    return fmaI(S{ a[2] }, S{ b[2] }, fmaI(S{ a[1] }, S{ b[1] }, S{ a[0] } *S{ b[0] }));
  }

  template<typename M, typename N> requires FloatOnlyVec3Common<M, N>
  [[nodiscard]] constexpr auto computeCross(const M& a, const N& b) noexcept
  {
    using S = CommonTypeOfScalars<M, N>;

    return Vector<S, 3>{ differenceOfProducts(S{ a[1] }, S{ b[2] }, S{ a[2] }, S{ b[1] }), differenceOfProducts(S{ a[2] }, S{ b[0] }, S{ a[0] }, S{ b[2] }), differenceOfProducts(S{ a[0] }, S{ b[1] }, S{ a[1] }, S{ b[0] }) };
  }

  template<typename M, typename N> requires FloatOnlyVec3Common<M, N>
  [[nodiscard]] constexpr auto absDot(const M& a, const N& b) noexcept
  {
    return std::abs(computeDot(a, b));
  }

  template<FloatOnlyVector3Like V>
  [[nodiscard]] V normalize(const V& v) noexcept
  {
    using S = ScalarOf<V>;

    const S lenSq{ computeDot(v, v) };

    if (lenSq > S{ 0 })
    {
      const S inv{ S{ 1 } / std::sqrt(lenSq) };

      return V{ v * inv };
    }

    return V{ S{ 0 } };
  }
}