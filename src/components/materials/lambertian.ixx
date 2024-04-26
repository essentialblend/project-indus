export module lambertian;

import material;
import color;

export class MLambertian : public Material
{
public:
	explicit MLambertian() noexcept = default;
	explicit MLambertian(const Color& albedo) noexcept;
	bool handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, Color& attenuation, Ray& scatteredRay) const override;

private:
	Color m_albedo{};
};

MLambertian::MLambertian(const Color& albedo) noexcept : m_albedo{ albedo } {}

bool MLambertian::handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, Color& attenuation, Ray& scatteredRay) const
{
	Vec3 scatterDirection = hitRec.normalVec + genRandomUnitSphereVecNorm();

	if (scatterDirection.isNearZero()) scatterDirection = hitRec.normalVec;
	scatteredRay = Ray(hitRec.hitPoint, scatterDirection);
	attenuation = m_albedo;
	return true;
}