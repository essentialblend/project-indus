#pragma once


class MDielectric : public Material
{
public:
	MDielectric(const double refInd) : refractionIndex(refInd) {}

	bool handleRayScatter(const Ray& inputRay, Ray& scatteredRay, const HitRecord& hitRec, ColorVec3& colorAttenuation) const override
	{
		colorAttenuation = ColorVec3(1.F);
		double refractionRatio = hitRec.frontFace ? (1.f / refractionIndex) : refractionIndex;

		Vec3 unitDirectionVec = computeUnitVector(inputRay.getRayDirection());
		Vec3 refractionDirection = computeRefractionDirection(unitDirectionVec, hitRec.hitNormalVec, refractionRatio);

		scatteredRay = Ray(hitRec.hitPoint, refractionDirection);

		return true;
	}

private:
	double refractionIndex;

};