export module material;

import <memory>;

import core_constructs;
import world_object;
import ray;
import hit_record;
import vec3;
import bxdf;

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