export module ray;

import vec3;

export class Ray
{
public:
	explicit Ray() noexcept = default;
	explicit Ray(const Point& rayOrigin, const Vec3& rayDir) noexcept;

	[[nodiscard]] Point getPointOnRayAt(double t) const noexcept;
	[[nodiscard]] Point getOrigin() const noexcept;
	[[nodiscard]] Vec3 getDirection() const noexcept;

private:
	Point m_rayOrigin{};
	Vec3 m_rayDirection{};
};