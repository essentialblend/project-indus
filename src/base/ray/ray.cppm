import ray;

Ray::Ray(const PointOLD& rayOrigin, const Vec3& rayDir) noexcept : m_rayOrigin(rayOrigin), m_rayDirection(rayDir) {};

PointOLD Ray::getPointOnRayAt(double t) const noexcept
{
	return m_rayOrigin + (t * m_rayDirection);
}

PointOLD Ray::getOrigin() const noexcept
{
	return PointOLD(m_rayOrigin);
}

Vec3 Ray::getDirection() const noexcept
{
	return Vec3(m_rayDirection);
}