export module indus.core.geom.ray;

import indus.core.types;
import indus.core.math.constants.i;
import indus.core.math.fp.ii;

export class Ray final
{
public:
  constexpr Ray() noexcept = default;
  constexpr Ray(const Point3f& origin, const Vec3f& direction, Float shutterTime, Float tMax) noexcept;

  [[nodiscard]] constexpr const Point3f& getOrigin() const noexcept;
  [[nodiscard]] constexpr const Vec3f& getDirection() const noexcept;
  [[nodiscard]] constexpr const Float getTimeSeconds() const noexcept;
  [[nodiscard]] constexpr Point3f getPointAt(Float) const noexcept;

  [[nodiscard]] constexpr Float getTMax() const noexcept;
  [[nodiscard]] constexpr const Vec3f& getInvDirection() const noexcept;
  [[nodiscard]] constexpr const std::array<int, 3>& getDirIsNeg() const noexcept;

  constexpr void setTMax(Float) noexcept;

private:
  Point3f m_origin{};
  Vec3f m_direction{};
  Float m_shutterTime{};

  Float m_tMax{ infinity<Float> };
  Vec3f m_invDir{};
  std::array<int, 3> m_dirIsNeg{};

  constexpr void recomputeCache() noexcept;
};

constexpr Ray::Ray(const Point3f& origin, const Vec3f& direction, Float shutterTime = 0, Float tMax = infinity<Float>) noexcept : m_origin{ origin }, m_direction{ direction }, m_shutterTime{ shutterTime }, m_tMax{ tMax }
{
  recomputeCache();
}

constexpr const Point3f& Ray::getOrigin() const noexcept
{
  return m_origin;
}

constexpr const Vec3f& Ray::getDirection() const noexcept
{
  return m_direction;
}

constexpr const Float Ray::getTimeSeconds() const noexcept
{
  return m_shutterTime;
}

constexpr Point3f Ray::getPointAt(Float t) const noexcept
{
  return m_origin + (m_direction * t);
}

constexpr Float Ray::getTMax() const noexcept
{
  return m_tMax;
}

constexpr const Vec3f& Ray::getInvDirection() const noexcept
{
  return m_invDir;
}

constexpr const std::array<int, 3>& Ray::getDirIsNeg() const noexcept
{
  return m_dirIsNeg;
}

constexpr void Ray::setTMax(Float t) noexcept
{
  m_tMax = t;
}

// We accept +- inf, so avoiding denom check is intentional
constexpr void Ray::recomputeCache() noexcept 
{

  m_invDir = Vec3f{ Float{ 1 } / m_direction[0], Float{ 1 } / m_direction[1], Float{ 1 } / m_direction[2] };

  m_dirIsNeg = { m_invDir[0] < Float{ 0 }, m_invDir[1] < Float{ 0 }, m_invDir[2] < Float{ 0 } };
}



