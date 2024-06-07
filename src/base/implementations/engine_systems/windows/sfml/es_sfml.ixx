export module es_sfml;

import <vector>;
import <future>;

import core_constructs;
import es_window;
import eu_timer;
import ri_singleton;
import es_overlaymanager;
import eu_overlay;


export class ESSFMLWindow final : public ESWindow, public RISingleton<ESSFMLWindow>
{
public:

	void initializeEntity([[maybe_unused]] const std::vector<std::any>& args = {}) override;

	void startEntity() override;
	void stopEntity() override;

	void updateWindow([[maybe_unused]] const std::vector<std::any>& args = {}) override;
	void processInputEvents([[maybe_unused]] const std::vector<std::any>& args = {}) override;
	void drawWindowContent([[maybe_unused]] const std::vector<std::any>& args = {}) override;

protected:
	void startPDHQuery() override;
	void stopPDHQuery() override;

private:
	// Private member variables.
	SFMLWindowProperties m_propertiesObj{};
	std::vector<std::shared_ptr<EUTimer>> m_timerList{};
	std::vector<std::shared_ptr<EUOverlay>> m_overlayList{};
	
	std::future<void> m_renderAsyncFuture{};
	bool m_isRendering{ false };
	bool m_needsDrawUpdate{ false };
	int m_texUpdateChunkTracker{ 0 };

	// Private member functions.
	bool retrievePDHQueryValues();
	void updateStatsOverlay();
	void setupPDHQueryAndCounter(PDHQueryCounterVars& pdhQueryAndCounter, const std::wstring& queryAPIString);
	void getFormattedValue();
	double retrieveTotalDRAM();
	void drawIdleScreen();

	// Special class members.
	explicit ESSFMLWindow() noexcept = default;
	ESSFMLWindow(const ESSFMLWindow&) = delete;
	ESSFMLWindow(ESSFMLWindow&&) noexcept = delete;
	ESSFMLWindow& operator=(const ESSFMLWindow&) = delete;
	ESSFMLWindow& operator=(ESSFMLWindow&&) noexcept = delete;
	~ESSFMLWindow() noexcept = default;

	friend class RISingleton<ESSFMLWindow>;
};