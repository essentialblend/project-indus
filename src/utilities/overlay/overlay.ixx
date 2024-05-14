export module overlay;

import<memory>;

import core_constructs;
import timer;

import <SFML/Graphics.hpp>;

export class Overlay abstract
{
public:
	explicit Overlay() noexcept = default;

	virtual void setupOverlay(bool isMultithreaded) = 0;

	virtual void setDefaultDisplayedText(bool isMultithreaded) = 0;
	virtual void setSFMLTextProperties() = 0;
	virtual void setRenderingCompleteStatus(bool hasRenderCompleted) noexcept = 0;
	virtual void setOverlayVisibility(bool shouldDisplayOverlay) noexcept = 0;

	virtual bool getOverlayVisibility() const noexcept = 0;

	virtual void showOverlay(sf::RenderWindow& renderWindowObj, const PixelResolution& pixResObj, const Timer& timerObj) = 0;
	
	virtual ~Overlay() noexcept = default;
};






