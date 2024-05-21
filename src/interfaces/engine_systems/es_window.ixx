export module es_window;

import <vector>;
import <any>;

import i_enginesystem;
import ri_runnable;
import ri_initializable;
import ri_configurable;

import core_constructs;

export class ESWindow abstract : public IEngineSystem, public RIRunnable  
{
public:
	explicit ESWindow() noexcept = default;

	virtual void updateWindow(const std::vector<std::any>& args) = 0;
	virtual void processInputEvents(const std::vector<std::any>& args) = 0;

	virtual void drawGUI(const std::vector<std::any>& args) = 0;

	virtual ~ESWindow() noexcept = default;

protected:
	virtual void startPDHQuery(PDHVariables&) = 0;
	virtual void stopPDHQuery(PDHVariables&) = 0;
};
