export module overlay;

import<memory>;

import core_constructs;
import timer;

import <SFML/Graphics.hpp>;

export class Overlay
{
public:
	explicit Overlay() noexcept = default;

	void setupOverlay();

	//void showOverlay(sf::RenderWindow& renderWindowObj, const PixelResolution& pixResObj, const Timer& timerObj);

    void setRenderingStatus(bool isRendering) noexcept
	{
		m_isCurrentlyRendering = isRendering;
	}

private:
    OverlayProperties m_overlayProps{};
    sf::Font m_overlayFont{};
	bool m_isOverlayEnabled{ true };
	bool m_isCurrentlyRendering{ true };
};

void Overlay::setupOverlay()
{
	if (!m_overlayFont.loadFromFile("src/dep/CourierPrime-Regular.ttf"))
	{
		m_isOverlayEnabled = false;
		return;
	}

	std::vector<std::reference_wrapper<OverlayStatistic>> localStatsCollection{ m_overlayProps.getStatsCollection() };

	for (auto& stat : localStatsCollection)
	{
		stat.get().statTitleObj.setFont(m_overlayFont);
		stat.get().statTitleObj.setCharacterSize(25);
		stat.get().statTitleObj.setColor(sf::Color::White);

		stat.get().statResultObj.setFont(m_overlayFont);
		stat.get().statResultObj.setCharacterSize(25);
		stat.get().statResultObj.setColor(sf::Color::White);
	}

	 m_overlayProps.modeObj.statTitleObj.setString("Mode: ");
	 m_overlayProps.timeObj.statTitleObj.setString("Time: ");
	 m_overlayProps.renderObj.statTitleObj.setString("Status: ");
	 m_overlayProps.GUIObj.statTitleObj.setString("GUI Status: ");
}

//void Overlay::showOverlay(sf::RenderWindow& renderWindowObj, const PixelResolution& pixResObj, const Timer& timerObj)
//{
//    if (!m_isOverlayEnabled)
//    {
//        return;
//    }
//
//    if (m_isRendering)
//    {
//
//    }
//    else
//    {
//
//    }
//}