export module metal;

import material;

export class MMetal : public Material
{
public:
	explicit MMetal() noexcept = default;
	explicit MMetal(const Color& albedo) noexcept;
	bool handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, Color& attenuation, Ray& scatteredRay) const override;

private:
	Color m_albedo{};
};

MMetal::MMetal(const Color& albedo) noexcept : m_albedo{ albedo } {}

bool MMetal::handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, Color& attenuation, Ray& scatteredRay) const
{
	Vec3 reflectedRayVec{ inputRay.getDirection() - (2 * computeDot(inputRay.getDirection(), hitRec.normalVec) * hitRec.normalVec) };
	scatteredRay = Ray(hitRec.hitPoint, reflectedRayVec);
	attenuation = m_albedo;
	return true;
}