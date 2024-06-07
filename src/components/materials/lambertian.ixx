export module lambertian;

import <memory>;

import material;
import color;
import vec3;


export class MLambertian : public IMaterial
{
public:
	explicit MLambertian() noexcept = default;
	explicit MLambertian(const std::shared_ptr<IColor> albedo) noexcept;
	bool handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const override;

private:
	std::shared_ptr<IColor> m_albedo{};
};

MLambertian::MLambertian(const std::shared_ptr<IColor> albedo) noexcept : m_albedo{ albedo } {}

bool MLambertian::handleRayScatter([[maybe_unused]] const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const
{
    Vec3 scatterDirection = hitRec.normalVec + genRandomUnitSphereVecNorm();

    if (scatterDirection.isNearZero()) scatterDirection = hitRec.normalVec;

    scatteredRay = Ray(hitRec.hitPoint, scatterDirection);
    albedo.setColor(*m_albedo);

    return true;
}