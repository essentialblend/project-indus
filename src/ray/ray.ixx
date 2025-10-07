export module ray;

import std;
import vector;
import point;
import types;

export class Ray final
{
public:
  constexpr Ray() noexcept = default;
  constexpr Ray(const Point3f& origin, const Vec3f& direction, Float shutterTime) noexcept;

  [[nodiscard]] constexpr const Point3f& getOrigin() const noexcept;
  [[nodiscard]] constexpr const Vec3f& getDirection() const noexcept;
  [[nodiscard]] constexpr const Float getTimeSeconds() const noexcept;
  [[nodiscard]] constexpr Point3f getPointAt(Float) const noexcept;


private:
  Point3f m_origin{};
  Vec3f m_direction{};
  Float m_shutterTime{};
};

constexpr Ray::Ray(const Point3f& origin, const Vec3f& direction, Float shutterTime = 0) noexcept : m_origin{ origin }, m_direction{ direction }, m_shutterTime{ shutterTime } {}

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



