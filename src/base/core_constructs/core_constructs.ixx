export module core_constructs;

import color;
import vec3;
import <vector>;

import<functional>;
import<Pdh.h>;

import <SFML/Graphics.hpp>;

export struct PixelResolution
{
	int widthInPixels{};
	int heightInPixels{};
};

export struct AspectRatio
{
	double widthInAbsVal{};
	double heightInAbsVal{};
};

export struct ViewportProperties
{
	double widthInWorldSpaceUnits{};
	double heightInWorldSpaceUnits{ 2.0 };
	Vec3 horizontalPixelSpan{};
	Vec3 verticalPixelSpan{};
};

export struct ImageProperties
{
	PixelResolution pixelResolutionObj{};
	AspectRatio aspectRatioObj{};
	int numColorChannels{3};
};

export struct PixelDimension
{
	Vec3 lateralSpanInAbsVal{};
	Vec3 verticalSpanInAbsVal{};
	Point pixelCenter{};
};

export struct CameraProperties
{
	ImageProperties camImgPropsObj{};
	ViewportProperties camViewportPropsObj{};
	PixelDimension camPixelDimObj{};

	Point camCenter{ 0.0, 0.0, 0.0 };
	Vec3 camFocalLength{0, 0, 1};
};

export struct SFMLWindowProperties
{
	sf::RenderWindow renderWindowObj{};
	sf::View viewObj{};
	sf::Texture texObj{};
	sf::Sprite spriteObj{};
	unsigned int prefFPSInIntegral{ 60 };
	float windowedResScale{ 0.5 };
};

export struct OverlayStatistic
{
	sf::Text statTitleObj{};
	sf::Text statResultObj{};
};

export struct OverlayProperties
{
	OverlayStatistic titleObj{};
	OverlayStatistic modeObj{};
	OverlayStatistic renderObj{};
	OverlayStatistic timeObj{};
	OverlayStatistic GUIObj{};
	OverlayStatistic cpuUsageObj{};

	std::vector<std::reference_wrapper<OverlayStatistic>> getStatsCollection()
	{
		return { std::ref(titleObj), std::ref(modeObj), std::ref(renderObj), std::ref(timeObj), std::ref(GUIObj), std::ref(cpuUsageObj) };
	}
};

export struct RendererSFMLFunctors
{
	std::function<void()> sfmlClearWindowFunctor{};
	std::function<void()> sfmlDrawSpriteFunctor{};
	std::function<void()> sfmlDisplayWindowFunctor{};
	std::function<void(const sf::Uint8*, unsigned int, unsigned int, unsigned int, unsigned int)> sfmlTextureUpdateFunctor{};
};

export struct RendererFunctors
{

};

export struct RenderingMode
{
	bool isSingleThreaded{ false };
	bool isMultiThreaded{ true };
};

export struct WindowFunctors
{
	std::function<void()> renderFrameSingleCoreFunctor{};
	std::function<void()> renderFrameMultiCoreFunctor{};
	std::function<bool()> isMultithreadedFunctor{};
	std::function<bool()> isTextureReadyForUpdateFunctor{};
	std::function<std::vector<Color>()> getMainEngineFramebufferFunctor{};
	std::function<std::pair<int, int>()> getTextureUpdateCounterFunctor{};
	std::function<CameraProperties()> getRendererCameraPropsFunctor{};
};

export struct PDHVariables
{
	HQUERY cpuQuery{};
	HCOUNTER cpuTotal{};
	PDH_FMT_COUNTERVALUE counterValue{};
};