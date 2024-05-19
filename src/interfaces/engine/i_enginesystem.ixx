export module i_enginesystem;

export class IEngineSystem abstract
{
public:
	IEngineSystem() noexcept = default;

	virtual void initializeEngineSystem() = 0;
	virtual void configureSystemComponents() = 0;
	virtual void cleanupSystemResources() = 0;

	virtual void startEngineSystem() = 0;
	virtual void stopEngineSystem() = 0;

	virtual ~IEngineSystem() noexcept = default;
};