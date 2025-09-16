export module matte;

import <memory>;

import material;
import vector;
import core_sampling_util;
import core_util;
import bsdf;
import lambertianBxDF;
import colorrgb;

export class MMatte final : public IMaterial
{
public:
	explicit MMatte(const ColorRGB&) noexcept;

	void computeScatteringFunctions(HitRecord&) const override;

	virtual MaterialType getMaterialType() const noexcept override;

private:
	ColorRGB m_reflectance{};
};

MMatte::MMatte(const ColorRGB& reflectance) noexcept : m_reflectance{ reflectance } {}

void MMatte::computeScatteringFunctions(HitRecord& hitRec) const
{
	hitRec.surfaceBSDF = std::make_unique<BSDF>(hitRec.shadingBasis);
	hitRec.surfaceBSDF->clearBxDFs();

	hitRec.surfaceBSDF->addBxDF(std::make_unique<LambertianBxDF>(m_reflectance));
}

MaterialType MMatte::getMaterialType() const noexcept
{
	return MaterialType::Matte;
}