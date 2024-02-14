#pragma once

class WOSphere : public WorldObject
{
public:
	WOSphere(PointVec3 sphC, double sphR, std::shared_ptr<Material> assignMat) : sphereCenter(sphC), sphereRadius(sphR), sphereMaterial(assignMat) {}

	virtual bool checkHit(const Ray& inputRay, Interval validInterval, HitRecord& hitRec) const override
	{
		Vec3 sphereCenterToRayOriginDir = inputRay.getRayOrigin() - sphereCenter;
		// Quadratic coeffs: a = b . b, b = 2b . (A - C), c = (A - C) . (A - C) - r^2.
		double a{ inputRay.getRayDirection().computeMagnitudeSquared() };
		double half_b{ computeDotProduct(sphereCenterToRayOriginDir, inputRay.getRayDirection()) };
		double c{ sphereCenterToRayOriginDir.computeMagnitudeSquared() - (sphereRadius * sphereRadius) };
		double discr{ (half_b * half_b) - (a * c) };

		if (discr < 0) return false;

		double solutionRoot{ (-half_b - std::sqrt(discr)) / a };
		if (!validInterval.isWithinInterval(solutionRoot))
		{
			solutionRoot = (-half_b + std::sqrt(discr)) / a;
			if (!validInterval.isWithinInterval(solutionRoot))
			{
				return false;
			}
		}
		hitRec.hitRoot = solutionRoot;
		hitRec.hitPoint = inputRay.getPointOnRayAt(hitRec.hitRoot);
		Vec3 outwardNormal = (hitRec.hitPoint - sphereCenter) / sphereRadius;
		hitRec.setFaceNormal(inputRay, outwardNormal);
		hitRec.hitMaterial = sphereMaterial;
		
		return true;
	}


private:
	PointVec3 sphereCenter;
	double sphereRadius;
	std::shared_ptr<Material> sphereMaterial;
};