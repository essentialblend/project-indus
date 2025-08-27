export module metal;

import <memory>;

import material;
import core_sampling_util;

export class MMetal : public IMaterial
{
public:
	explicit MMetal() noexcept = default;
	explicit MMetal(ColorRGB albedo, double fuzzFactor) noexcept;
	bool handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, ColorRGB& albedo, Ray& scatteredRay, double& PDFValue) const override;

private:
	ColorRGB m_albedo{};
	double m_metalFuzz{};
	void reflectRay(const Ray& inputRay, const HitRecord& hitRec, ColorRGB& albedo, Ray& scatteredRay) const;
};

MMetal::MMetal(ColorRGB albedo, double fuzzFactor) noexcept : m_albedo{ albedo }, m_metalFuzz{ fuzzFactor } {}

bool MMetal::handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, ColorRGB& albedo, Ray& scatteredRay, double& PDFValue) const
{
	reflectRay(inputRay, hitRec, albedo, scatteredRay);
	
  // For metal materials, we assume the PDF is uniform.
	PDFValue = 1.0;
	
	return (computeDot(scatteredRay.getDirection(), hitRec.unitNormalVec) > 0);
}

void MMetal::reflectRay(const Ray& inputRay, const HitRecord& hitRec, ColorRGB& albedo, Ray& scatteredRay) const
{
	albedo = m_albedo;
	Vec3 reflectedRayVec{ inputRay.getDirection() - (2 * computeDot(inputRay.getDirection(), hitRec.unitNormalVec) * hitRec.unitNormalVec) };
	reflectedRayVec = getUnit(reflectedRayVec) + (m_metalFuzz * genRandomUnitSphereVec());
	scatteredRay = Ray(hitRec.hitPoint, reflectedRayVec);
}