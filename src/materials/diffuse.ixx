export module diffuse;

import material;
import vector;
import samplingutil;
import bsdf_new;
import lambertianBxDF;
import colorrgb;
import surfaceinteraction;

export class Diffuse final : public Material
{
public:
	explicit Diffuse(const ColorRGB&) noexcept;

	virtual BSDF getBSDF(const SurfaceInteraction& si) const override;

	virtual MaterialType getMaterialType() const noexcept override;

private:
	ColorRGB m_reflectance{};
};

Diffuse::Diffuse(const ColorRGB& reflectance) noexcept : m_reflectance{ reflectance } {}

BSDF Diffuse::getBSDF(const SurfaceInteraction& surfaceInteraction) const
{
	BSDF bsdf{ surfaceInteraction.getShadingBasis() };
	bsdf.setBxDF(std::make_unique<LambertianBxDF>(m_reflectance));
	
	return bsdf;
}

MaterialType Diffuse::getMaterialType() const noexcept
{
	return MaterialType::Diffuse;
}