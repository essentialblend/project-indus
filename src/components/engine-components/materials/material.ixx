export module material;

import bsdf_new;
import colorrgb;

export enum class MaterialType
{
	Diffuse,
	Glass
};

export class Material
{
public:
	virtual ~Material() = default;
	virtual MaterialType getMaterialType() const noexcept = 0;
	virtual ColorRGB getReflectance() const noexcept = 0;
};