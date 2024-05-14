import ray;

Ray::Ray(const Point& rayOrigin, const Vec3& rayDir) noexcept : m_rayOrigin(rayOrigin), m_rayDirection(rayDir) {};

Point Ray::getPointOnRayAt(double t) const noexcept
{
	return m_rayOrigin + (t * m_rayDirection);
}

Point Ray::getOrigin() const noexcept
{
	return Point(m_rayOrigin);
}

Vec3 Ray::getDirection() const noexcept
{
	return Vec3(m_rayDirection);
}