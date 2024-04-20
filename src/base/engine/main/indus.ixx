export module indus;

import core_constructs;
import vec3;
import renderer;
import camera;
import overlay;
import window;

export class Indus
{
public:
	explicit Indus() noexcept = default;
	
	explicit Indus(const PixelResolution& windowPixResObj, const PixelResolution& imagePixResObj, const AspectRatio& aspectRatioObj) noexcept;
	
	void initializeEngine();
	void runEngine();

private:
	SFMLWindow m_mainWindow{};
	Renderer m_mainRenderer{};
	StatsOverlay m_statsOverlay{};
	ImageProperties m_mainRenderImageProps{};
	std::vector<Color> m_mainRenderFramebuffer{};
	bool m_isMultithreaded{ true };

	void setGlobalCallbackFunctors();
};