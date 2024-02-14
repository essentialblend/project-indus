#pragma once


class MLambertian : public Material
{
public:
	MLambertian(const ColorVec3& albedo) : albedoValue(albedo) {}

	bool handleRayScatter(const Ray& inputRay, Ray& scatteredRay, const HitRecord& hitRec, ColorVec3& colorAttenuation) const override
	{
		Vec3 newReflRayDir{ hitRec.hitNormalVec + genNormalizedRandVec3UnitSphere() };
		if (newReflRayDir.checkNearZero()) { newReflRayDir = hitRec.hitNormalVec; }
		scatteredRay = Ray(hitRec.hitPoint, newReflRayDir);
		colorAttenuation = albedoValue;
		return true;
	}

private:
	ColorVec3 albedoValue;

};