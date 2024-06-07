export module sphere;

import <memory>;

import world_object;
import vec3;
import material;

export class WOSphere : public WorldObject
{
public:
	explicit WOSphere() noexcept = default;
	explicit WOSphere(const PointOLD& center, double radius, std::shared_ptr<IMaterial> material) noexcept;

	bool checkHit(const Ray& inputRay, Interval rayInterval, HitRecord& hitRec) const override;

private:
	PointOLD m_sphereCenter{};
	double m_sphereRadius{};
	std::shared_ptr<IMaterial> m_sphereMaterial{};
};