#pragma once

class Material
{
public:
	virtual ~Material() = default;
	virtual bool handleRayScatter(const Ray& inputRay, Ray& scatteredRay, const HitRecord& hitRec, ColorVec3& colorAttenuation) const = 0;
};