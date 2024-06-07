export module es_renderer;

import <vector>;
import <memory>;
import <any>;

import eu_color;
import i_enginesystem;
import ri_runnable;
import ri_initializable;

export class ESRenderer abstract : public IEngineSystem, public RIStartable
{
public:
	ESRenderer() noexcept = default;
	virtual const EUColor& computePixelSampleColor(const std::vector<std::any>& args) = 0;
	virtual ~ESRenderer() noexcept = default;
};
