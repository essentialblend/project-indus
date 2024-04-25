export module sphere;

import world_object;
import vec3;

export class WOSphere : public WorldObject
{
public:
	explicit WOSphere() noexcept = default;
	explicit WOSphere(const Point& center, double radius) noexcept;

	bool checkHit(const Ray& inputRay, Interval rayInterval, HitRecord& hitRec) const override;

private:
	Point m_sphereCenter{};
	double m_sphereRadius{};
};