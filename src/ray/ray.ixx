export module ray;

import vector;
import point;
import types;

export class Ray final
{
public:
  constexpr Ray() noexcept = default;
  constexpr Ray(const Point3f&, const Vec3f&) noexcept;

  constexpr const Point3f& getOrigin() const noexcept;
  constexpr const Vec3f& getDirection() const noexcept;

  constexpr Point3f getPointAt(Float) const noexcept;

private:
  Point3f m_origin{};
  Vec3f m_direction{};
};

constexpr Ray::Ray(const Point3f& o, const Vec3f& d) noexcept : m_origin{ o }, m_direction{ d } {}

constexpr const Point3f& Ray::getOrigin() const noexcept
{
  return m_origin;
}

constexpr const Vec3f& Ray::getDirection() const noexcept
{
  return m_direction;
}

constexpr Point3f Ray::getPointAt(Float t) const noexcept
{
  return m_origin + (m_direction * t);
}




