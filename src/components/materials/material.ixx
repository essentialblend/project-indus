export module material;

import <memory>;

import world_object;
import ray;
import hit_record;
import color;

export class IMaterial abstract
{
public:
	explicit IMaterial() noexcept = default;
	virtual bool handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, IColor& albedo, Ray& scatteredRay) const = 0;
	
	virtual ~IMaterial() noexcept = default;
};