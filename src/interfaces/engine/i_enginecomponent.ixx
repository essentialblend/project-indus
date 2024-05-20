export module i_enginecomponent;

import ri_initializable;

export class IEngineComponent abstract
{
public:
	IEngineComponent() noexcept = default;

	virtual ~IEngineComponent() noexcept = default;
};