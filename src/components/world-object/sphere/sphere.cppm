import sphere;

import core_util;

WOSphere::WOSphere(const Point& center, double radius, std::shared_ptr<IMaterial> material) noexcept : m_sphereCenter(center), m_sphereRadius(radius), m_sphereMaterial{ material } {}

bool WOSphere::checkHit(const Ray& inputRay, Interval rayInterval, HitRecord& hitRec) const
{
	Vec3 originToCenter{ m_sphereCenter - inputRay.getOrigin() };

	double a{ inputRay.getDirection().getMagnitudeSq() };
	double half_b{ -2.0 * computeDot(inputRay.getDirection(), originToCenter) };
	double c{ originToCenter.getMagnitudeSq() - (m_sphereRadius * m_sphereRadius) };

	auto rootsOpt = evaluateQuadratic(a, half_b, c);
	if (!rootsOpt) return false;

	auto& [t0, t1] = *rootsOpt;

	double root{ t0 };
	if (!rayInterval.isSurrounded(root))
	{
		root = t1;
		if (!rayInterval.isSurrounded(root))
		{
			return false;
		}
	}

	hitRec.root = root;
  hitRec.hitPoint = inputRay.getPointOnRayAt(root);
	
	Vec3 unitOutNormal{ (hitRec.hitPoint - m_sphereCenter) / m_sphereRadius };
  hitRec.setFaceNormal(inputRay, unitOutNormal);
	hitRec.hitMaterial = m_sphereMaterial;

	return true;
}