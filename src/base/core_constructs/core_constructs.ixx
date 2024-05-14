export module core_constructs;

import color;
import vec3;
import color;
import ray;
import threadpool;

import <vector>;
import <future>;
import<functional>;
import<Pdh.h>;


import <SFML/Graphics.hpp>;

export struct PixelResolution
{
	int widthInPixels{};
	int heightInPixels{};
	long long totalPixels{ widthInPixels * heightInPixels };
};

export struct AspectRatio
{
	double widthInAbsVal{};
	double heightInAbsVal{};
};

export struct ViewportProperties
{
	double widthInWorldSpaceUnits{};
	double heightInWorldSpaceUnits{};
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
	Point topLeftmostPixelCenter{};
	double pixelUnitSpanInAbsVal{};
};

export struct CameraProperties
{
	ImageProperties camImgPropsObj{};
	ViewportProperties camViewportPropsObj{};
	PixelDimension camPixelDimObj{};

	double camVerticalFOV{ 20 };
	Point camLookFrom{ 13, 2, 3 };
	Point camCenter{ camLookFrom };
	Point camLookAt{ 0, 0, 0 };
	Vec3 camVUP{ 0, 1, 0 };
	

	Vec3 camW{ getUnit(camLookFrom - camLookAt) }; Vec3 camU{ getUnit(computeCross(camVUP, camW)) }; Vec3 camV{ computeCross(camW, camU) };
	double defocusAngle{0.6};
	double focusDist{10};
	Vec3 defocusDiskU{};
	Vec3 defocusDiskV{};
};

export struct SFMLWindowProperties
{
	sf::RenderWindow renderWindowObj{};
	sf::View mainRenderViewObj{};
	sf::View mainOverlayViewObj{};
	sf::Texture mainRenderTexObj{};
	sf::Sprite mainRenderSpriteObj{};
	unsigned int prefFPSInIntegral{ 30 };
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
	OverlayStatistic cpuUsageObj{};
	OverlayStatistic dramUsageObj{};

	std::vector<std::reference_wrapper<OverlayStatistic>> getStatsCollection()
	{
		return { std::ref(titleObj), std::ref(modeObj), std::ref(timeObj), std::ref(renderObj), std::ref(cpuUsageObj), std::ref(dramUsageObj)};
	}
};

export struct RendererSFMLFunctors
{
	std::function<void()> sfmlClearWindowFunctor{};
	std::function<void()> sfmlDrawSpriteFunctor{};
	std::function<void()> sfmlDisplayWindowFunctor{};
	std::function<void(const sf::Uint8*, unsigned int, unsigned int, unsigned int, unsigned int)> sfmlTextureUpdateFunctor{};
};

export struct RenderingMode
{
	bool isSingleThreaded{ false };
	bool isMultiThreaded{ true };
};

export struct WindowFunctors
{
	std::function<void()> renderFrameMultiCoreFunctor{};
	std::function<bool()> isMultithreadedFunctor{};
	std::function<bool()> isTextureReadyForUpdateFunctor{};
	std::function<std::vector<std::unique_ptr<IColor>>&()> getMainEngineFramebufferFunctor{};
	std::function<CameraProperties()> getRendererCameraPropsFunctor{};
	std::function<bool()> getRenderCompleteStatusFunctor{};
	std::function<int()> getTextureUpdateRateFunctor{};
	std::function<MT_ThreadPool&()> getThreadPoolFunctor{};
};

export struct PDHQueryCounterVars
{
	HQUERY pdhQueryObj{};
	HCOUNTER pdhCounterObj{};
	PDH_FMT_COUNTERVALUE pdhFmtCounterValObj{};
};

export struct PDHVariables
{
	PDHQueryCounterVars totalCPUUsage{};
	PDHQueryCounterVars availDRAM{};

	std::vector<std::reference_wrapper<PDHQueryCounterVars>> getPDHObjects()
	{
		return { std::ref(totalCPUUsage), std::ref(availDRAM) };
	}
};

export struct GaussianKernelProperties
{
	double sigmaInAbsVal{};
	int kernelSpanInIntegralVal{};
	int kernelCoverageScalar{ 3 };
};