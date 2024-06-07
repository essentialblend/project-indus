export module eu_overlay;

import i_engineutility;
import ri_initializable;

export class EUOverlay abstract : public IEngineUtility, public RIInitializable
{
public:
	EUOverlay() noexcept = default;

	virtual void flipDisplayStatus() noexcept = 0;
	virtual void showOverlay([[maybe_unused]] const std::vector<std::any>& args = {}) = 0;
	virtual void updateOverlayContent([[maybe_unused]] const std::vector<std::any>& args = {}) = 0;
	virtual bool getOverlayVisibility() const noexcept = 0;

	~EUOverlay() noexcept = default;
};