export module ray;

import vec3;

export class Ray
{
public:
	explicit Ray() noexcept = default;
	explicit Ray(const PointOLD& rayOrigin, const Vec3& rayDir) noexcept;

	[[nodiscard]] PointOLD getPointOnRayAt(double t) const noexcept;
	[[nodiscard]] PointOLD getOrigin() const noexcept;
	[[nodiscard]] Vec3 getDirection() const noexcept;

private:
	PointOLD m_rayOrigin{};
	Vec3 m_rayDirection{};
};