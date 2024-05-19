export module es_gui;

import i_enginesystem;

export class ESGUI : public IEngineSystem
{
public:
	ESGUI() noexcept = default;

	virtual void initializeEngineSystem() override = 0;
	virtual void configureSystemComponents() override = 0;
	virtual void cleanupSystemResources() override = 0;

	~ESGUI() noexcept = default;
};
