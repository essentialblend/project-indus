export module eu_statsoverlay;

import eu_overlay;
import h_overlaystatistic;

import <SFML/Graphics.hpp>;
import <string>;

export class EUStatsOverlay final : public EUOverlay
{
public:
	explicit EUStatsOverlay() noexcept = default;
	EUStatsOverlay(const EUStatsOverlay&) = default;
	EUStatsOverlay& operator=(const EUStatsOverlay&) = default;
	EUStatsOverlay(EUStatsOverlay&&) noexcept = default;
	EUStatsOverlay& operator=(EUStatsOverlay&&) noexcept = default;

	void initializeEntity([[maybe_unused]] const std::vector<std::any>& args = {}) override;
	void flipDisplayStatus() noexcept override;
	void showOverlay([[maybe_unused]] const std::vector<std::any>& args = {}) override;
	virtual void updateOverlayContent([[maybe_unused]] const std::vector<std::any>& args = {}) override;
	bool getOverlayVisibility() const noexcept override;

	std::pair<sf::Text, sf::Text>& getOverlayTitleResultPairMutable() noexcept;
	
	~EUStatsOverlay() noexcept = default;

private:
	std::vector<HOverlayStatistic> m_overlayStats{};
	std::vector<std::pair<std::string, std::string>> m_defaultStatsContentVector
	{
		{"Engine", "Indus."},
		{"Version", "0.0.1-alpha."},
		{"Status", "Idle."},
		{"Total CPU", "N.A."},
		{"Total DRAM", "N.A."},
		{"Runtime", "N.A."}
	};

	std::pair<sf::Text, sf::Text> m_overlayTitleResultPair{};
	sf::Font m_overlayFont{};
	bool m_displayStatus{ true };
	double m_totalCPUUsage{};
	double m_totalDRAMGB{};
	double m_usedDRAMGB{};
	
	void addStatistic(const std::string_view titleStr, const std::string_view resultStr);
	std::pair<std::string, std::string> getFormattedStrForPDHVars();
	void setSFMLTextObjectProperties();
};

