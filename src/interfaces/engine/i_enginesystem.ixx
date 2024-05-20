export module i_enginesystem;

import ri_initializable;

export class IEngineSystem abstract : public RIInitializable
{
public:
	IEngineSystem() noexcept = default;

	virtual void initializeEntity() = 0;

	virtual ~IEngineSystem() noexcept = default;
};