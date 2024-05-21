export module i_engine;

import ri_initializable;
import ri_configurable;
import ri_runnable;


export class IEngine abstract : public RIInitializable, public RIConfigurable, public RIRunnable
{
public:
	IEngine() noexcept = default;
	virtual ~IEngine() noexcept = default;
};