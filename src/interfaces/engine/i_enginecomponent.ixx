export module i_enginecomponent;

export class IEngineComponent abstract
{
public:
	IEngineComponent() noexcept = default;
	virtual ~IEngineComponent() noexcept = default;
};