export module metal;

import <memory>;

import material;
import color;

export class MMetal : public IMaterial
{
public:
	explicit MMetal() noexcept = default;
	explicit MMetal(const std::shared_ptr<IColor> albedo, double fuzzFactor) noexcept;
	bool handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const override;

private:
	std::shared_ptr<IColor> m_albedo{};
	double m_metalFuzz{};
	void reflectRay(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const;
};

MMetal::MMetal(const std::shared_ptr<IColor> albedo, double fuzzFactor) noexcept : m_albedo{ albedo }, m_metalFuzz{ fuzzFactor } {}

bool MMetal::handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const
{
	reflectRay(inputRay, hitRec, albedo, scatteredRay);
	
	return (computeDot(scatteredRay.getDirection(), hitRec.normalVec) > 0);
}

void MMetal::reflectRay(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const
{
	albedo.setColor(*m_albedo);
	Vec3 reflectedRayVec{ inputRay.getDirection() - (2 * computeDot(inputRay.getDirection(), hitRec.normalVec) * hitRec.normalVec) };
	reflectedRayVec = getUnit(reflectedRayVec) + (m_metalFuzz * genRandomUnitSphereVec());
	scatteredRay = Ray(hitRec.hitPoint, reflectedRayVec);
}