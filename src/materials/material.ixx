export module material;

import surfaceinteraction;
import bsdf_new;

export enum class MaterialType
{
	Diffuse,
	Glass
};

export class Material
{
public:
	virtual ~Material() = default;

	virtual BSDF getBSDF(const SurfaceInteraction& si) const = 0;

	virtual MaterialType getMaterialType() const noexcept = 0;
};