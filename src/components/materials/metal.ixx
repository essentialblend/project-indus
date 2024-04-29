export module metal;

import material;

export class MMetal : public Material
{
public:
	explicit MMetal() noexcept = default;
	explicit MMetal(const Color& albedo, double fuzzFactor) noexcept;
	bool handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, Color& attenuation, Ray& scatteredRay) const override;

private:
	Color m_albedo{};
	double m_metalFuzz{};
	void reflectRay(const Ray& inputRay, const HitRecord& hitRec, Color& attenuation, Ray& scatteredRay) const;
};

MMetal::MMetal(const Color& albedo, double fuzzFactor) noexcept : m_albedo{ albedo }, m_metalFuzz{ fuzzFactor } {}

bool MMetal::handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, Color& attenuation, Ray& scatteredRay) const
{
	reflectRay(inputRay, hitRec, attenuation, scatteredRay);
	
	return (computeDot(scatteredRay.getDirection(), hitRec.normalVec) > 0);
}

void MMetal::reflectRay(const Ray& inputRay, const HitRecord& hitRec, Color& attenuation, Ray& scatteredRay) const
{
	attenuation = m_albedo;
	Vec3 reflectedRayVec{ inputRay.getDirection() - (2 * computeDot(inputRay.getDirection(), hitRec.normalVec) * hitRec.normalVec) };
	reflectedRayVec = getUnit(reflectedRayVec) + (m_metalFuzz * genRandomUnitSphereVec());
	scatteredRay = Ray(hitRec.hitPoint, reflectedRayVec);
}