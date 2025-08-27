export module matte;

import <memory>;

import material;
import vec3;
import core_sampling_util;
import core_util;
import bsdf;
import lambertianBxDF;

export class MMatte final : public IMaterial
{
public:
	explicit MMatte(const ColorRGB& reflectance) noexcept : m_reflectance{ reflectance } {}

	void computeScatteringFunctions(HitRecord& hitRec) const override
	{
		hitRec.surfaceBSDF = std::make_unique<BSDF>(hitRec.shadingBasis);
		hitRec.surfaceBSDF->clearBxDFs();

		hitRec.surfaceBSDF->addBxDF(std::make_unique<LambertianBxDF>(m_reflectance));
	}

	virtual MaterialType getMaterialType() const noexcept override
	{
		return MaterialType::Matte;
	}

private:
	ColorRGB m_reflectance{};
};