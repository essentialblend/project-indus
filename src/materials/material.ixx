export module material;

import <memory>;

import hit_record;

export enum class MaterialType
{
	Matte,
	Glass
};

export class IMaterial
{
public:
	virtual ~IMaterial() = default;

	virtual void computeScatteringFunctions(HitRecord& hitRec) const = 0;

	virtual MaterialType getMaterialType() const noexcept = 0;
};