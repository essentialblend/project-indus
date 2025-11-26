export module indus.shading.material;

import indus.core.colorrgb;

export enum class MaterialType
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