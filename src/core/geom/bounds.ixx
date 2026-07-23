export module indus.core.geom.bounds;

import indus.core.types;
import indus.core.concepts;

import indus.core.geom.vector;
import indus.core.geom.point;
import indus.core.geom.ray;

import indus.core.math.fp.ii;

export template<ScalarLike T, std::size_t N> requires Arity23<N>
class Bounds final
{
public:
  using value_type = T;

  constexpr Bounds() noexcept;
  constexpr explicit Bounds(const Point<T, N>&) noexcept;

  constexpr explicit Bounds(const Point<T, N>&, const Point<T, N>&) noexcept;

  template<ScalarLike U>
  constexpr explicit Bounds(const Bounds<U, N>&) noexcept;

  [[nodiscard]] constexpr const Point<T, N>& getMin() const noexcept;
  [[nodiscard]] constexpr const Point<T, N>& getMax() const noexcept;

  [[nodiscard]] constexpr Point<T, N> getCorner(int) const noexcept;
  [[nodiscard]] constexpr Vector<T, N> getDiagonal() const noexcept;
  [[nodiscard]] constexpr int getMaxDimension() const noexcept;
  [[nodiscard]] constexpr Point<T, N> lerp(const Point<T, N>&) const noexcept requires FloatScalarLike<T>;
  [[nodiscard]] constexpr Vector<T, N> offset(const Point<T, N>&) const noexcept requires FloatScalarLike<T>;
  [[nodiscard]] constexpr bool isEmpty() const noexcept;
  [[nodiscard]] constexpr bool isDegenerate() const noexcept;

  [[nodiscard]] constexpr T getArea() const noexcept requires Arity2<N>;

  [[nodiscard]] constexpr T getSurfaceArea() const noexcept requires Arity3<N>;

  [[nodiscard]] constexpr T getVolume() const noexcept requires Arity3<N>;

  static constexpr Bounds getUnion(const Bounds&, const Point<T, N>&) noexcept;
  static constexpr Bounds getUnion(const Bounds&, const Bounds&) noexcept;
  [[nodiscard]] constexpr Bounds getIntersect(const Bounds&) const noexcept;
  
  [[nodiscard]] static constexpr bool overlaps(const Bounds&, const Bounds&) noexcept;
  [[nodiscard]] static constexpr bool inside(const Point<T, N>&, const Bounds&) noexcept;
  [[nodiscard]] static constexpr bool insideExclusive(const Point<T, N>&, const Bounds&) noexcept;

  template<FloatingArithmetic U>
  [[nodiscard]] static constexpr decltype(U{} - T{}) getDistanceSq(const Point<U, N>&, const Bounds&) noexcept;

  template<FloatingArithmetic U>
  [[nodiscard]] static constexpr decltype(U{} - T{}) getDistance(const Point<U, N>&, const Bounds&) noexcept;
  
  template<FloatingArithmetic U>
  [[nodiscard]] static constexpr Bounds expandBoundsByDelta(const Bounds&, U) noexcept requires FloatScalarLike<T>;

private:
  Point<T, N> m_min{};
  Point<T, N> m_max{};
};

export using Bounds2f = Bounds<Float, 2>;
export using Bounds2i = Bounds<Int, 2>;

export using Bounds3f = Bounds<Float, 3>;
export using Bounds3i = Bounds<Int, 3>;

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr Bounds<T, N>::Bounds() noexcept 
{
  const T lo{ std::numeric_limits<T>::lowest() };
  const T hi{ std::numeric_limits<T>::max() };

  for (std::size_t i{}; i < N; ++i)
  { 
    m_min[i] = hi; m_max[i] = lo; 
  }
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr Bounds<T, N>::Bounds(const Point<T, N>& point) noexcept
  : m_min(point), m_max(point) {}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr Bounds<T, N>::Bounds(const Point<T, N>& minPoint, const Point<T, N>& maxPoint) noexcept 
{
  for (std::size_t i{}; i < N; ++i)
  {
    const T a{ minPoint[i] };
    const T b{ maxPoint[i] };

    m_min[i] = (a < b) ? a : b;
    m_max[i] = (a < b) ? b : a;
  }
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
template<ScalarLike U>
constexpr Bounds<T, N>::Bounds(const Bounds<U, N>& b) noexcept 
{
  if (b.isEmpty()) 
  { 
    *this = Bounds{}; 
    return; 
  }
  
  for (std::size_t i{}; i < N; ++i)
  {
    m_min[i] = static_cast<T>(b.getMin()[i]);
    m_max[i] = static_cast<T>(b.getMax()[i]);
  }
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr const Point<T, N>& Bounds<T, N>::getMin() const noexcept
{
  return m_min;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr const Point<T, N>& Bounds<T, N>::getMax() const noexcept
{
  return m_max;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr Point<T, N> Bounds<T, N>::getCorner(int i) const noexcept 
{
  Point<T, N> p{};
  
  for (std::size_t k{}; k < N; ++k) 
    p[k] = (i & (1 << k)) ? m_max[k] : m_min[k];

  return p;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr Vector<T, N> Bounds<T, N>::getDiagonal() const noexcept 
{
  return m_max - m_min;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr int Bounds<T, N>::getMaxDimension() const noexcept
{
  const Vector<T, N> d{ getDiagonal() };
  
  if constexpr (N == 2) 
  {
    return (d[0] > d[1]) ? 0 : 1;
  }
  else if constexpr (N == 3) [[likely]]
  {
    return (d[0] > d[1]) ? ((d[0] > d[2]) ? 0 : 2) : ((d[1] > d[2]) ? 1 : 2);
  }
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr Point<T, N> Bounds<T, N>::lerp(const Point<T, N>& pointToLerp) const noexcept requires FloatScalarLike<T>
{
  Point<T, N> p{};
  
  for (std::size_t i{}; i < N; ++i)
  {
    const T t{ clampUnit(static_cast<T>(pointToLerp[i])) };

    p[i] = m_min[i] + (t * (m_max[i] - m_min[i]));
  }
  
  return p;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr Vector<T, N> Bounds<T, N>::offset(const Point<T, N>& p) const noexcept requires FloatScalarLike<T>
{
  Vector<T, N> o{};

  for (std::size_t i{}; i < N; ++i) 
  {
    const T e{ m_max[i] - m_min[i] };
    
    o[i] = (e > T{ 0 }) ? (p[i] - m_min[i]) / e : T{ 0 };
  }

  return o;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr bool Bounds<T, N>::isEmpty() const noexcept 
{
  for (std::size_t i{}; i < N; ++i)
    if (m_min[i] > m_max[i]) return true;
  
  return false;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr bool Bounds<T, N>::isDegenerate() const noexcept 
{
  // Degenerate if any extent == 0 (valid but measure-zero box)
  for (std::size_t i = 0; i < N; ++i) 
    if (m_min[i] == m_max[i]) return true;
  
  return false;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr T Bounds<T, N>::getArea() const noexcept requires Arity2<N>
{
  if (isEmpty()) return T{ 0 };

  const Vector<T, 2> e{ getDiagonal() };

  return e[0] * e[1];
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr T Bounds<T, N>::getSurfaceArea() const noexcept requires Arity3<N> 
{
  if (isEmpty()) return T{ 0 };

  const T x{ m_max[0] - m_min[0] };
  const T y{ m_max[1] - m_min[1] };
  const T z{ m_max[2] - m_min[2] };
  
  return T{ 2 } * (x * y + x * z + y * z);
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr T Bounds<T, N>::getVolume() const noexcept requires Arity3<N> 
{
  if (isEmpty()) return T{ 0 };

  const T x{ m_max[0] - m_min[0] };
  const T y{ m_max[1] - m_min[1] };
  const T z{ m_max[2] - m_min[2] };
  
  return x * y * z;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr Bounds<T, N> Bounds<T, N>::getUnion(const Bounds& b, const Point<T, N>& p) noexcept 
{
  if (b.isEmpty()) return Bounds{ p };
  
  Point<T, N> mn{};
  Point<T, N> mx{};
  
  for (std::size_t i{}; i < N; ++i)
  {
    mn[i] = (p[i] < b.m_min[i]) ? p[i] : b.m_min[i];
    mx[i] = (p[i] > b.m_max[i]) ? p[i] : b.m_max[i];
  }

  return Bounds{ mn, mx };
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr Bounds<T, N> Bounds<T, N>::getUnion(const Bounds& a, const Bounds& b) noexcept 
{
  if (a.isEmpty()) return b;
  if (b.isEmpty()) return a;
  
  Point<T, N> mn{};
  Point<T, N> mx{};

  for (std::size_t i{}; i < N; ++i)
  {
    mn[i] = (a.m_min[i] < b.m_min[i]) ? a.m_min[i] : b.m_min[i];
    mx[i] = (a.m_max[i] > b.m_max[i]) ? a.m_max[i] : b.m_max[i];
  }

  return Bounds{ mn, mx };
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr Bounds<T, N> Bounds<T, N>::getIntersect(const Bounds& b) const noexcept 
{
  if (isEmpty() || b.isEmpty()) return Bounds{};

  Point<T, N> mn{}, mx{};
  
  for (std::size_t i{}; i < N; ++i)
  {
    mn[i] = (m_min[i] > b.m_min[i]) ? m_min[i] : b.m_min[i];
    mx[i] = (m_max[i] < b.m_max[i]) ? m_max[i] : b.m_max[i];
  }

  for (std::size_t i = 0; i < N; ++i) 
    if (mn[i] > mx[i]) return Bounds{};

  return Bounds{ mn, mx };
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr bool Bounds<T, N>::overlaps(const Bounds& a, const Bounds& b) noexcept 
{
  for (std::size_t i = 0; i < N; ++i)
    if (a.m_max[i] < b.m_min[i] || a.m_min[i] > b.m_max[i]) return false;
  
  return true;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr bool Bounds<T, N>::inside(const Point<T, N>& p, const Bounds& b) noexcept 
{
  for (std::size_t i{}; i < N; ++i)
    if (p[i] < b.m_min[i] || p[i] > b.m_max[i]) return false;
  
  return true;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
constexpr bool Bounds<T, N>::insideExclusive(const Point<T, N>& p, const Bounds& b) noexcept 
{
  for (std::size_t i = 0; i < N; ++i)
    if (p[i] < b.m_min[i] || p[i] >= b.m_max[i]) return false;

  return true;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
template<FloatingArithmetic U>
constexpr decltype(U{} - T{}) Bounds<T, N>::getDistanceSq(const Point<U, N>& p, const Bounds& b) noexcept 
{
  using RT = decltype(U{} - T{});
  
  RT sum{};
  
  for (std::size_t i{}; i < N; ++i)
  {
    RT dx{ 0 };

    if (p[i] < b.m_min[i]) dx = RT{ b.m_min[i] - p[i] };
    else if (p[i] > b.m_max[i]) dx = RT{ p[i] - b.m_max[i] };
    
    sum += dx * dx;
  }
  
  return sum;
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
template<FloatingArithmetic U>
constexpr decltype(U{} - T{}) Bounds<T, N>::getDistance(const Point<U, N>& p, const Bounds& b) noexcept
{
  using RT = decltype(U{} - T{});
  
  return RT(std::sqrt(RT(getDistanceSq(p, b))));
}

template<ScalarLike T, std::size_t N> requires Arity23<N>
template<FloatingArithmetic U>
constexpr Bounds<T, N> Bounds<T, N>::expandBoundsByDelta(const Bounds& a, U delta) noexcept requires FloatScalarLike<T>
{
  Point<T, N> mn{};
  Point<T, N> mx{};
  
  const T d{ static_cast<T>(delta) };

  for (std::size_t i{}; i < N; ++i)
  { 
    mn[i] = a.m_min[i] - d; 
    mx[i] = a.m_max[i] + d; 
  }
  
  return Bounds{ mn, mx };
}

//template<ScalarLike T, std::size_t N> requires Arity23<N>
//constexpr std::optional<RayBoxHit> Bounds<T, N>::intersectPRange(const Ray& ray) const noexcept requires Arity3<N> && FloatScalarLike<T>
//{
//  if (isEmpty()) return std::nullopt;
//
//  T t0{ T{0} };
//  T t1{ ray.getTMax() };
//  const auto& rayOrigin{ ray.getOrigin() };
//  const auto& rayInvDir{ ray.getInvDirection() };
//
//  for (int i{}; i < 3; ++i) 
//  {
//    T tNear{ (m_min[i] - rayOrigin[i]) * rayInvDir[i] };
//    T tFar{ (m_max[i] - rayOrigin[i]) * rayInvDir[i] };
//    
//    if (tNear > tFar) std::swap(tNear, tFar);
//    
//    tFar *= T{ 1 } + T{ 2 } * gamma<T>(3);
//    
//    t0 = (tNear > t0) ? tNear : t0;
//    t1 = (tFar < t1) ? tFar : t1;
//    
//    if (t0 >= t1) return std::nullopt;
//  }
//  
//  return RayBoxHit{ t0, t1 };
//}
//
//template<ScalarLike T, std::size_t N> requires Arity23<N>
//constexpr SphereBounds Bounds<T, N>::getBoundingSphere() const noexcept  requires Arity3<N> 
//{
//  if (isEmpty()) return { {}, T{0} };
//  
//  Point<T, 3> c{}; 
//  for (int i{}; i < 3; ++i)
//    c[i] = (m_min[i] + m_max[i]) * T{ 0.5 };
//  
//  const T dx{ c[0] - m_max[0] };
//  const T dy{ c[1] - m_max[1] };
//  const T dz{ c[2] - m_max[2] };
//
//  return { c, std::sqrt(dx * dx + dy * dy + dz * dz) };
//}