export module indus.geom.bounds_intersection;

import indus.core.concepts;
import indus.core.types;

import indus.core.geom.ray;
import indus.core.geom.bounds;

export
{
  struct RayBoxHit
  {
    Float tEnter{};
    Float tExit{};
  };

  struct SphereBounds
  {
    Point3f sphereCenter;
    Float sphereRadius{};
  };

  template<ScalarLike T>
  constexpr std::optional<RayBoxHit> intersectPRange(const Bounds<T, 3>& b, const Ray& ray) requires FloatScalarLike<T>;

  constexpr SphereBounds getBoundingSphere(const Bounds3f& b) noexcept;

};

template<ScalarLike T>
constexpr std::optional<RayBoxHit> intersectPRange(const Bounds<T, 3>& b, const Ray& ray) requires FloatScalarLike<T>
{
  if (b.isEmpty()) return std::nullopt;

  T t0{ T{0} };
  T t1{ ray.getTMax() };

  const auto& o{ ray.getOrigin() };
  const auto& inv{ ray.getInvDirection() };

  for (Int i{}; i < 3; ++i)
  {
    T tNear{ (b.getMin()[i] - o[i]) * inv[i] };
    T tFar{ (b.getMax()[i] - o[i]) * inv[i] };

    if (tNear > tFar) std::swap(tNear, tFar);

    tFar *= T{ 1 } + T{ 2 } * gamma<T>(3);

    t0 = tNear > t0 ? tNear : t0;
    t1 = tFar < t1 ? tFar : t1;

    if (t0 >= t1) return std::nullopt;
  }

  return RayBoxHit{ t0, t1 };
}

constexpr SphereBounds getBoundingSphere(const Bounds3f& b) noexcept
{
  if (b.isEmpty()) return { {}, Float{} };

  Point3f c{};

  for (Int i{}; i < 3; ++i) c[i] = (b.getMin()[i] + b.getMax()[i]) * Float{ 0.5 };

  const Float dx{ c[0] - b.getMax()[0] };
  const Float dy{ c[1] - b.getMax()[1] };
  const Float dz{ c[2] - b.getMax()[2] };

  return { c, std::sqrt(dx * dx + dy * dy + dz * dz) };
}