import sphere;

import <cmath>;

WOSphere::WOSphere(const Point& center, double radius) noexcept : m_sphereCenter(center), m_sphereRadius(radius) {}

bool WOSphere::checkHit(const Ray& inputRay, Interval rayInterval, HitRecord& hitRec) const
{
    Vec3 originToCenter = m_sphereCenter - inputRay.getOrigin();
    auto a = inputRay.getDirection().getMagnitudeSq();
    auto h = computeDot(inputRay.getDirection(), originToCenter);
    auto c = originToCenter.getMagnitudeSq() - (m_sphereRadius * m_sphereRadius);
    auto discriminant = (h * h) - (a * c);

    if (discriminant < 0) return false;
    
    auto discrSqrt{ std::sqrt(discriminant) };
    auto root = (h - discrSqrt) / a;
    if(!rayInterval.isSurrounded(root))
	{
		root = (h + discrSqrt) / a;
		if(!rayInterval.isSurrounded(root))
		{
			return false;
		}
	}
    hitRec.root = root;
	hitRec.hitPoint = inputRay.getPointOnRayAt(hitRec.root);
	Vec3 outwardNormal = (hitRec.hitPoint - m_sphereCenter) / m_sphereRadius;
	hitRec.setFaceNormal(inputRay, outwardNormal);
	return true;
}