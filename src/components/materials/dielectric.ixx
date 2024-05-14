export module dielectric;

import <cmath>;
import <typeinfo>;
import <print>;

import material;
import core_util;
import color_rgb;

export class MDielectric : public IMaterial
{
public:
	explicit MDielectric() noexcept = default;
	explicit MDielectric(const double refractionIndex) noexcept;
	bool handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const override;

private:
	// Dielectrics don't need albedo, but included for polymorphism, and consistency.
	std::shared_ptr<IColor> m_albedo{};
	double m_refractionIndex{};

	void reflectRay(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const;
	void refractRay(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const;
	static double computeShlicksReflectance(double cosine, double refractionIndex);
};

MDielectric::MDielectric(const double refractionIndex) noexcept : m_refractionIndex{ refractionIndex }, m_albedo{ std::make_shared<ColorRGB>(1.0) } {}

bool MDielectric::handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const
{
	const double refractionRatio{ hitRec.hitFrontFace ? (1.0 / m_refractionIndex) : m_refractionIndex };
	const Vec3 inputRayUnitVec{ getUnit(inputRay.getDirection()) };

	const auto cosineTheta{ std::fmin(computeDot(-inputRayUnitVec, hitRec.normalVec), 1.0) };
	const auto sinTheta{ std::sqrt(1.0 - cosineTheta * cosineTheta) };

	bool cannotRefract{ refractionRatio * sinTheta > 1.0 };

	if (cannotRefract || computeShlicksReflectance(cosineTheta, refractionRatio) > UGenRNG<double>())
		reflectRay(inputRay, hitRec, albedo, scatteredRay);
	else 
		refractRay(inputRay, hitRec, albedo, scatteredRay);

	return true;
}

void MDielectric::reflectRay(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const
{
	albedo.setColor(*m_albedo);
    const Vec3 reflectedRayVec{ inputRay.getDirection() - (2 * computeDot(inputRay.getDirection(), hitRec.normalVec) * hitRec.normalVec) };
    scatteredRay = Ray(hitRec.hitPoint, reflectedRayVec);
}

void MDielectric::refractRay(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const
{
	albedo.setColor(*m_albedo);

	const double refractionRatio{ hitRec.hitFrontFace ? (1.0 / m_refractionIndex) : m_refractionIndex };
	const Vec3 inputRayUnitVec{ getUnit(inputRay.getDirection()) };

	auto cosineTheta{ std::fmin(computeDot(-inputRayUnitVec, hitRec.normalVec), 1.0) };
	const Vec3 rOutPerp{ refractionRatio * (inputRayUnitVec + cosineTheta * hitRec.normalVec) };
	const Vec3 rOutParallel{ -std::sqrt(std::fabs(1.0 - rOutPerp.getMagnitudeSq())) * hitRec.normalVec };
	scatteredRay = Ray(hitRec.hitPoint, rOutPerp + rOutParallel);
}

double MDielectric::computeShlicksReflectance(double cosine, double refractionIndex)
{
	const auto rZero{ (1.0 - refractionIndex) / (1.0 + refractionIndex) };
	const auto rZSq{ rZero * rZero };
	return rZSq + ((1 - rZSq) * std::pow((1 - cosine), 5));
}
