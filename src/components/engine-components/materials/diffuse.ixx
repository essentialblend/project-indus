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
	virtual MaterialType getMaterialType() const noexcept override;
	virtual ColorRGB getReflectance() const noexcept override;


private:
	ColorRGB m_reflectance{};
};

Diffuse::Diffuse(const ColorRGB& reflectance) noexcept : m_reflectance{ reflectance } {}

MaterialType Diffuse::getMaterialType() const noexcept
{
	return MaterialType::Diffuse;
}

ColorRGB Diffuse::getReflectance() const noexcept
{
	return m_reflectance;
}
