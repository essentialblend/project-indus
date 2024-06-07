export module i_engine;

import <vector>;
import <memory>;

import color;
import ri_initializable;
import ri_runnable;
import ec_framebuffer;

export class IEngine abstract : public RIInitializable, public RIRunnable
{
public:
	IEngine() noexcept = default;

	[[nodiscard]] virtual const ECFramebuffer& getEngineFramebuffer() const noexcept = 0;
	virtual void setCallbackFunctors() = 0;
	
	virtual ~IEngine() noexcept = default;
};