export module indus;

import <vector>;
import <memory>;

import core_constructs;
import vec3;
import renderer;
import color;
import camera;
import overlay;
import window;

import world_object;

export class Indus
{
public:
	explicit Indus() noexcept = default;
	
	explicit Indus(const PixelResolution& windowPixResObj, const PixelResolution& imagePixResObj, const AspectRatio& aspectRatioObj) noexcept;
	
	void initializeEngine();
	void runEngine();

	void setRendererFunctors();
	void setWindowFunctors();

	[[nodiscard]] std::vector<std::unique_ptr<IColor>>& getMainRenderFramebuffer() noexcept;

private:
	SFMLWindow m_mainWindow{};
	Renderer m_mainRenderer{};
	StatsOverlay m_statsOverlay{};
	ImageProperties m_mainRenderImageProps{};
	std::vector<std::unique_ptr<IColor>> m_mainRenderFramebuffer{};
	bool m_isMultithreaded{ true };

	WorldObjectList m_mainWorld{};

	void setGlobalCallbackFunctors();
	void initializeWorld();
};