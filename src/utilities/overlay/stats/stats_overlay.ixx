export module stats_overlay;

import overlay;

export class StatsOverlay : public Overlay
{
public:
	explicit StatsOverlay() noexcept = default;

	virtual void setupOverlay(bool isMultithreaded) override;
	virtual void setDefaultDisplayedText(bool isMultithreaded) override;
	virtual void setSFMLTextProperties() override;
	virtual void setRenderingCompleteStatus(bool hasRenderCompleted) noexcept override;
	virtual void setOverlayVisibility(bool shouldDisplayOverlay) noexcept override;
	
	void setTotalCPUUsage(double totalCPUUsage) noexcept;

	[[nodiscard]] virtual bool getOverlayVisibility() const noexcept override;

	virtual void showOverlay(sf::RenderWindow& renderWindowObj, const PixelResolution& pixResObj, const Timer& timerObj) override;

private:
	OverlayProperties m_overlayProps{};
	sf::Font m_overlayFont{};
	bool m_isOverlayEnabled{ true };
	bool m_hasRenderCompleted{ false };
	bool m_hasRenderStarted{ false };
	double m_totalCPUUsage{};
};