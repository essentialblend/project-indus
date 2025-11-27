export module indus.shading.material;

import std;

import indus.core.colorrgb;

export enum class MaterialType : std::uint8_t
{
	Diffuse,
	Glass,
	CoatedDiffuse
};

export class Material
{
public:
	virtual ~Material() = default;
	virtual MaterialType getMaterialType() const noexcept = 0;
	virtual ColorRGB getReflectance() const noexcept = 0;
};