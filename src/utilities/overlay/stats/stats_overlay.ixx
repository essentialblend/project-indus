export module stats_overlay;

import overlay;

export class StatsOverlay : public Overlay
{
public:
	explicit StatsOverlay() noexcept = default;

	virtual void setupOverlay(bool isMultithreaded) override;
	[[noreturn]] virtual void setDefaultDisplayedText(bool isMultithreaded) override;
	[[noreturn]] virtual void setSFMLTextProperties() override;
	[[noreturn]] virtual void setRenderingStatus(bool hasRenderCompleted) noexcept override;
	[[noreturn]] virtual void setOverlayVisibility(bool shouldDisplayOverlay) noexcept override;

	[[nodiscard]] virtual bool getOverlayVisibility() const noexcept override;

	virtual void showOverlay(sf::RenderWindow& renderWindowObj, const PixelResolution& pixResObj, const Timer& timerObj) override;

private:
	OverlayProperties m_overlayProps{};
	sf::Font m_overlayFont{};
	bool m_isOverlayEnabled{ true };
	bool m_hasRenderCompleted{ false };
};