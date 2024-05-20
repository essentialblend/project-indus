export module i_engine;

import ri_initializable;
import ri_configurable;
import ri_runnable;


export class IEngine abstract : public RIInitializable, public RIConfigurable, public RIRunnable
{
public:
	IEngine() noexcept = default;
	
	virtual void initializeEntity() = 0;
	virtual void configureEntity() = 0;

	virtual void startEntity() = 0;
	virtual void stopEntity() = 0;

	virtual ~IEngine() noexcept = default;
};