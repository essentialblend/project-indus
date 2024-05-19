export module i_enginecomponent;

export class IEngineComponent abstract
{
public:
	IEngineComponent() noexcept = default;

	virtual void initializeEngineComponent() = 0;
	virtual void configureEngineComponent() = 0;
	virtual void cleanupComponentResources() = 0;

	virtual ~IEngineComponent() noexcept = default;
};