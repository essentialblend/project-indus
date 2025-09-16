export module indus;

import <vector>;
import <memory>;

import core_constructs;
import vec3;
import renderer;
import camera;
import overlay;
import window;

import world_object;

export class Indus
{
public:
	explicit Indus() noexcept = default;
	
	explicit Indus(const PixelResolution& windowPixResObj, const PixelResolution& imagePixResObj, const AspectRatio& aspectRatioObj, int SPP, int rayBounceDepth = 0, bool isDiagOutputEnabled = false, bool isRenderSavedToDisk = true, bool isMultithreaded = true) noexcept;
	
	void initializeEngine();
	void runEngine();

	void setRendererFunctors();
	void setWindowFunctors();

	void renderShirleyFirstBookScene();
	void renderLiTestScene();
	void renderFresnelTestScene();

	[[nodiscard]] std::vector<ColorRGB>& getMainRenderFramebuffer() noexcept;

private:
	SFMLWindow m_mainWindow{};
	Renderer m_mainRenderer{};
	StatsOverlay m_statsOverlay{};
	ImageProperties m_mainRenderImageProps{};
	std::vector<ColorRGB> m_mainRenderFramebuffer{};

	bool m_isMultithreaded{ true };
	int m_SPP{};
	int m_rayBounceDepth{};

	WorldObjectList m_mainWorld{};
	WorldObjectList m_LiTestWorld{};

	void setGlobalCallbackFunctors();
	void initializeWorld();
};