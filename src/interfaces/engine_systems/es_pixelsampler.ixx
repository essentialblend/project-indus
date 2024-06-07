export module es_pixelsampler;

import <vector>;
import <any>;

import i_enginesystem;
import h_ray;

export class ESPixelSampler abstract : public IEngineSystem
{
public:
	ESPixelSampler() noexcept = default;
	virtual HRay getRayForPixelSample([[maybe_unused]] const std::vector<std::any>& args = {}) = 0;
	virtual ~ESPixelSampler() noexcept = default;
};