export module i_engine;

export class IEngine abstract
{
public:
	IEngine() noexcept = default;
	
	virtual void initializeEngine() = 0;
	virtual void setupEngineSystems() = 0;
	virtual void runEngine() = 0;
	virtual void shutdownEngine() = 0;
	virtual void setupScene() = 0;

	virtual ~IEngine() noexcept = default;
};