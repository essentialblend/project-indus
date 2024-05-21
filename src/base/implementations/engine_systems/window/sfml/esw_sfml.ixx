export module esw_sfmlwindow;

import es_window;

export class ESSFMLWindow final : public ESWindow
{
public:

	virtual void initializeEntity() override;

	virtual void startEntity() override;
	virtual void stopEntity() override;

	virtual void updateWindow(const std::vector<std::any>& args) override;
	virtual void processInputEvents(const std::vector<std::any>& args) override;
	virtual void drawGUI(const std::vector<std::any>& args) override;

	static ESSFMLWindow& getSingletonInstance();
	
protected:
	virtual void startPDHQuery(PDHVariables&) override;
	virtual void stopPDHQuery(PDHVariables&) override;

private:
	SFMLWindowProperties m_propertiesObj{};

	bool m_isRendering{ false };
	bool m_needsDrawUpdate{ false };
	int m_texUpdateChunkTracker{ 0 };
	std::future<void> m_mainRenderSchedulerFuture{};

	bool retrievePDHQueryValues(PDHVariables& pdhVars);
	//void updatePDHOverlayPeriodic(StatsOverlay& statsOverlayObj, PDHVariables& pdhVars, double totalDRAMGigabytes);
	void setupPDHQueryAndCounter(PDHQueryCounterVars& pdhQueryAndCounter, const std::wstring& queryAPIString);
	void getFormattedValue(PDHVariables&);
	double retrieveTotalDRAM();


	// Special members.
	explicit ESSFMLWindow() noexcept = default;
	ESSFMLWindow(const ESSFMLWindow&) = delete;
	ESSFMLWindow(ESSFMLWindow&&) noexcept = delete;
	ESSFMLWindow& operator=(const ESSFMLWindow&) = delete;
	ESSFMLWindow& operator=(ESSFMLWindow&&) noexcept = delete;
	~ESSFMLWindow() noexcept = default;
};

