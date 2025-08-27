export module core_constructs;

import vec3;
import u_timer;
import ray;
import threadpool;
import es_threadpool;

import <vector>;
import <future>;
import <functional>;
import <Pdh.h>;


import <SFML/Graphics.hpp>;

export struct PixelResolution
{
	int widthInPixels{};
	int heightInPixels{};

	constexpr explicit PixelResolution() noexcept = default;
	constexpr explicit PixelResolution(const int widthPixels, const int heightPixels) noexcept : widthInPixels{ widthPixels }, heightInPixels{ heightPixels } {}

	constexpr long long getTotalPixels() const noexcept
	{
		return static_cast<long long>(widthInPixels * heightInPixels);
	}
};

export struct AspectRatio
{
	double widthAbsVal{};
	double heightAbsVal{};

	constexpr AspectRatio() noexcept = default;
	constexpr AspectRatio(const double widthAbsVal, const double heightAbsVal) noexcept : widthAbsVal{ widthAbsVal }, heightAbsVal{ heightAbsVal } {}

	constexpr double getAspectRatio() const noexcept
	{
		return widthAbsVal / heightAbsVal;
	}
};

export struct ViewportProperties
{
	double widthWorldSpaceUnits{};
	double heightWorldSpaceUnits{};
	Vec3 horPixelSpanVec{};
	Vec3 vertPixelSpanVec{};

	constexpr ViewportProperties() noexcept = default;
	constexpr ViewportProperties(const double widthInWorldSpace, const double heightInWorldSpace) noexcept : widthWorldSpaceUnits{ widthInWorldSpace }, heightWorldSpaceUnits{ heightInWorldSpace } {}
};

export struct ImageProperties
{
	PixelResolution pixelResolutionObj{};
	AspectRatio aspectRatioObj{};
	int numColorChannels{3};

	constexpr ImageProperties() noexcept = default;
	constexpr ImageProperties(const PixelResolution& pixResObj, const AspectRatio& arObj) noexcept : pixelResolutionObj{ pixResObj }, aspectRatioObj{ arObj } {}
};

export struct PixelDimension
{
	Vec3 lateralSpanVec{};
	Vec3 vertSpanVec{};
	Point topLeftPixCenter{};
	double pixelUnitSpanAbsVal{};
};

export struct DefocusBlurProperties
{
	double defocusAngle{ 0.55 };
	double focusDist{ 13 };
	Vec3 defocusDiskU{};
	Vec3 defocusDiskV{};
};

export struct OrthonormalBasis
{
public:
	Vec3 m_unitNormalVec{};
	Vec3 m_unitTangentVec{};
	Vec3 m_unitBitangentVec{};

	constexpr explicit OrthonormalBasis() noexcept = default;
	constexpr explicit OrthonormalBasis(Vec3 n, Vec3 t, Vec3 b) noexcept : m_unitNormalVec{ n }, m_unitTangentVec{ t }, m_unitBitangentVec{ b } {}
};

export struct CameraProperties
{
	ImageProperties camImgPropsObj{};
	ViewportProperties camViewportPropsObj{};
	PixelDimension camPixelDimObj{};
	DefocusBlurProperties camDefocusPropsObj{};

	double camVerticalFOV{ 18 };
	Point camLookFrom{ -15, 2.25, 8 };
	Point camCenter{ camLookFrom };
	Point camLookAt{ 0, 0, 0 };
	Vec3 camVUP{ 0, 1, 0 };

	Vec3 camW{ getUnit(camLookFrom - camLookAt) }; Vec3 camU{ getUnit(computeCross(camVUP, camW)) }; Vec3 camV{ computeCross(camW, camU) };

	void setLookFromAt(const Vec3& lookFrom, const Vec3& lookAt)
	{
		camLookFrom = lookFrom;
		camLookAt = lookAt;
	}
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

export struct GaussianKernelProperties
{
	double sigmaInAbsVal{};
	int kernelSpanInIntegralVal{};
	int kernelCoverageScalar{ 6 };
	std::vector<double> kernelWeights{};
};

export struct OldWindowFunctors
{
	std::function<void()> renderFrameFunctor{};
	std::function<void()> liTestRenderFrameFunctor{};
	std::function<bool()> isMultithreadedFunctor{};
	std::function<bool()> isTextureReadyForUpdateFunctor{};
	std::function<std::vector<ColorRGB>&()> getMainEngineFramebufferFunctor{};
	std::function<CameraProperties()> getRendererCameraPropsFunctor{};
	std::function<bool()> getRenderCompleteStatusFunctor{};
	std::function<int()> getTextureUpdateRateFunctor{};
	std::function<GaussianKernelProperties()> getGaussianKernelPropsFunctor{};
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

export struct EngineStatistics
{
	PixelResolution imageResObj{};
	AspectRatio aspectRatioObj{};
	PixelResolution windowResObj{};
	UTimer totalRuntimeTimerObj{};

	// Sampling mode details.
	// Filtering mode details.
	// Frame render time details.
	// System details.
	// Render status.
	// TBD
};

export struct SFMLAPIObjects
{
	sf::RenderWindow renderWindowObj{};
	sf::View mainRenderViewObj{};
	sf::View mainOverlayViewObj{};
	sf::Texture mainRenderTexObj{};
	sf::Sprite mainRenderSpriteObj{};
};

export struct SFMLWindowFunctors
{
	std::function<void()> renderFrameMultiCoreFunctor{};
	std::function<bool()> isMultithreadedFunctor{};
	std::function<bool()> isTextureReadyForUpdateFunctor{};
	std::function<std::vector<ColorRGB>&()> getMainEngineFramebufferFunctor{};
	//std::function<CameraProperties()> getRendererCameraPropsFunctor{};
	std::function<bool()> getRenderCompleteStatusFunctor{};
	std::function<int()> getTextureUpdateRateFunctor{};
	std::function<ESThreadpool&()> getThreadpoolFunctor{};
	std::function<GaussianKernelProperties()> getGaussianKernelPropsFunctor{};
	std::function<std::string()> getRenderColorTypeFunctor{};
};

export struct SFMLWindowProperties
{
	std::string windowTitle{ "indus prelim" };
	unsigned int windowFPSIntegral{ 30 };

	PDHVariables pdhVars{};
	SFMLAPIObjects sfmlAPIObj{};
	PixelResolution windowResolution{};
	SFMLWindowFunctors windowFunctors{};
	std::shared_ptr<UTimer> pdhTimer{};
};

export struct SFMLWindowPropertiesOLD
{
	sf::RenderWindow renderWindowObj{};
	sf::View mainRenderViewObj{};
	sf::View mainOverlayViewObj{};
	sf::Texture mainRenderTexObj{};
	sf::Sprite mainRenderSpriteObj{};
	unsigned int prefFPSInIntegral{ 30 };
	float windowedResScale{ 0.5 };
};

export struct Sample2D
{
	double u{}, v{};
};