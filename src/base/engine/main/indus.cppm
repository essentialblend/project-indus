import indus;

import color_rgb;
import sphere;
import material;
import lambertian;
import metal;
import dielectric;
import core_util;

Indus::Indus(const PixelResolution& windowPixResObj, const PixelResolution& imagePixResObj, const AspectRatio& aspectRatioObj) noexcept :
	m_mainWindow(windowPixResObj),
	m_mainRenderImageProps{imagePixResObj, aspectRatioObj} {};

void Indus::initializeWorld()
{
	const auto groundAlbedo{ std::make_shared<ColorRGB>(0.5) };
	auto groundMat = std::make_shared<MLambertian>(groundAlbedo);
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(PointOLD(0, -1000, 0), 1000, groundMat));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto matChosen = UGenRNG<double>();
			PointOLD center(a + 0.9 * UGenRNG<double>(), 0.2, b + 0.9 * UGenRNG<double>());

			if ((center - PointOLD(4, 0.2, 0)).getMagnitude() > 0.9) {
				std::shared_ptr<IMaterial> sphereMat;

				if (matChosen < 0.8) {
					// diffuse
					const auto matAlbedo{ ColorRGB(UGenRNG<double>(), UGenRNG<double>(), UGenRNG<double>()) * ColorRGB(UGenRNG<double>(), UGenRNG<double>(), UGenRNG<double>()) };
					sphereMat = std::make_shared<MLambertian>(std::make_shared<ColorRGB>(matAlbedo));
					m_mainWorld.addWorldObj(std::make_unique<WOSphere>(center, 0.2, sphereMat));
				}
				else if (matChosen < 0.95) {
					// metal
					const auto matAlbedo{ ColorRGB(UGenRNG<double>(0.5, 1), UGenRNG<double>(0.5, 1), UGenRNG<double>(0.5, 1)) };
					auto fuzz = UGenRNG<double>(0, 0.5);
					sphereMat = std::make_shared<MMetal>(std::make_shared<ColorRGB>(matAlbedo), fuzz);
					m_mainWorld.addWorldObj(std::make_unique<WOSphere>(center, 0.2, sphereMat));
				}
				else {
					// glass
					sphereMat = std::make_shared<MDielectric>(1.5);
					m_mainWorld.addWorldObj(std::make_unique<WOSphere>(center, 0.2, sphereMat));
				}
			}
		}
	}

	auto firstMat = std::make_shared<MDielectric>(1.5);
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(PointOLD(0, 1, 0), 1.0, firstMat));

	auto secondMat = std::make_shared<MLambertian>(std::make_shared<ColorRGB>((0.4, 0.2, 0.1)));
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(PointOLD(-4, 1, 0), 1.0, secondMat));

	auto thirdMat = std::make_shared<MMetal>(std::make_shared<ColorRGB>((0.7, 0.6, 0.5)), 0.0);
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(PointOLD(4, 1, 0), 1.0, thirdMat));
}

void Indus::initializeEngine()
{
	m_mainRenderer.setupRenderer(m_mainRenderImageProps.pixelResolutionObj, m_mainRenderImageProps.aspectRatioObj);
	m_mainRenderer.setThreadingMode(m_isMultithreaded);
	m_mainRenderFramebuffer.resize(m_mainRenderImageProps.pixelResolutionObj.getTotalPixels());
	m_statsOverlay.setupOverlay(m_isMultithreaded);
	setGlobalCallbackFunctors();
}

void Indus::setGlobalCallbackFunctors()
{
	setWindowFunctors();
	setRendererFunctors();
}

void Indus::runEngine()
{
	// Run window.
	EUTimer overlayTimer;
	initializeWorld();
	m_mainWindow.displayWindow(m_statsOverlay, overlayTimer);
}

void Indus::setRendererFunctors()
{
	RendererSFMLFunctors localCopy{};
	auto& mainDisplayTexObj{ m_mainWindow.getSFMLWindowProperties().mainRenderTexObj };

	localCopy.sfmlTextureUpdateFunctor = [&mainDisplayTexObj](const sf::Uint8* pixelData, unsigned int widthRegionInPixels, unsigned int heightRegionInPixels, unsigned int xCoord, unsigned int yCoord)
		{
			mainDisplayTexObj.update(pixelData, widthRegionInPixels, heightRegionInPixels, xCoord, yCoord);
		};
	localCopy.sfmlClearWindowFunctor = [this]() {m_mainWindow.getSFMLWindowProperties().renderWindowObj.clear(); };
	localCopy.sfmlDisplayWindowFunctor = [this]() {m_mainWindow.getSFMLWindowProperties().renderWindowObj.display(); };
	localCopy.sfmlDrawSpriteFunctor = [this]()
		{
			const auto& tempSprite = m_mainWindow.getSFMLWindowProperties().mainRenderSpriteObj;
			m_mainWindow.getSFMLWindowProperties().renderWindowObj.draw(tempSprite);
		};

	m_mainRenderer.setRendererSFMLFunctors(localCopy);
}

void Indus::setWindowFunctors()
{
	const std::function<bool()> multithreadingCheckFunctor{ [&]() { return m_mainRenderer.getThreadingMode(); } };

	const std::function<void()> renderMultiCoreFrameFunctor{ [&]()
	{
		m_mainRenderer.renderFrameMultiCoreGaussian(m_mainRenderFramebuffer, m_mainWorld);
	}};

	const std::function<bool()> texUpdateCheckFunctor{ [&]()
	{
		return m_mainRenderer.checkForDrawUpdate();
	}};

	const std::function<std::vector<std::unique_ptr<IColor>>&()> mainRenderFramebufferGetter{[&]()
	-> std::vector<std::unique_ptr<IColor>>& {
		return getMainRenderFramebuffer();
	}};

	const std::function<CameraProperties()> mainRendererCamPropsGetter{ [&]()
	{
		return m_mainRenderer.getRendererCameraProps();
	}};

	const std::function<bool()> getRenderCompleteStatusGetter{ [&]()
	{
		return m_mainRenderer.getRenderCompleteStatus();
	}};

	const std::function<int()> textureUpdateRateGetter{ [&]()
	{
		return m_mainRenderer.getTexUpdateRate();
	}};

	const std::function<GaussianKernelProperties()> gaussianKernelPropsGetter{ [&]()
	{
		return m_mainRenderer.getGaussianKernelProps();
	}};

	const std::function<std::string()> renderColorTypeGetter{ [&]()
	{
		return m_mainRenderer.getRenderColorType();
	}};

	m_mainWindow.setMainEngineFramebufferGetFunctor(mainRenderFramebufferGetter);
	m_mainWindow.setTextureUpdateCheckFunctor(texUpdateCheckFunctor);
	m_mainWindow.setMultithreadedCheckFunctor(multithreadingCheckFunctor);
	m_mainWindow.setRenderFrameMultiCoreFunctor(renderMultiCoreFrameFunctor);
	m_mainWindow.setMainRendererCameraPropsGetFunctor(mainRendererCamPropsGetter);
	m_mainWindow.setRenderCompleteStatusGetFunctor(getRenderCompleteStatusGetter);
	m_mainWindow.setTextureUpdateRateGetFunctor(textureUpdateRateGetter);
	m_mainWindow.setGaussianKernelPropsGetFunctor(gaussianKernelPropsGetter);
	m_mainWindow.setRenderColorTypeGetFunctor(renderColorTypeGetter);
}

std::vector<std::unique_ptr<IColor>>& Indus::getMainRenderFramebuffer() noexcept
{
	return m_mainRenderFramebuffer;
}

