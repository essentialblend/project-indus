export module material;

import world_object;
import ray;
import hit_record;
import color;

export class Material abstract
{
public:
	explicit Material() noexcept = default;
	virtual bool handleRayScatter(const Ray& inputRay, const HitRecord& hitRec, Color& attenuation, Ray& scatteredRay) const = 0;
	
	virtual ~Material() noexcept = default;
};