export module es_window;

import <vector>;
import <any>;

import i_enginesystem;
import ri_runnable;
import ri_initializable;

export class ESWindow abstract : public IEngineSystem, public RIRunnable  
{
public:
	ESWindow() noexcept = default;

	virtual void updateWindow([[maybe_unused]] const std::vector<std::any>& args = {}) = 0;
	virtual void processInputEvents([[maybe_unused]] const std::vector<std::any>& args = {}) = 0;
	virtual void drawWindowContent([[maybe_unused]] const std::vector<std::any>& args = {}) = 0;

	virtual ~ESWindow() noexcept = default;

protected:
	virtual void startPDHQuery() = 0;
	virtual void stopPDHQuery() = 0;
};
