import indus;

import color;
import sphere;
import material;
import lambertian;
import metal;
import dielectric;
import core_util;

Indus::Indus(const PixelResolution& windowPixResObj, const PixelResolution& imagePixResObj, const AspectRatio& aspectRatioObj) noexcept :
	m_mainWindow(windowPixResObj),
	m_mainRenderImageProps{
		.pixelResolutionObj{imagePixResObj},
		.aspectRatioObj {aspectRatioObj} } {};

void Indus::initializeWorld()
{
	auto groundMat = std::make_shared<MLambertian>(Color(0.5, 0.5, 0.5));
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(0, -1000, 0), 1000, groundMat));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto matChosen = UGenRNG<double>();
			Point center(a + 0.9 * UGenRNG<double>(), 0.2, b + 0.9 * UGenRNG<double>());

			if ((center - Point(4, 0.2, 0)).getMagnitude() > 0.9) {
				std::shared_ptr<Material> sphereMat;

				if (matChosen < 0.8) {
					// diffuse
					auto matAlbedo = Color(UGenRNG<double>(), UGenRNG<double>(), UGenRNG<double>()) * Color(UGenRNG<double>(), UGenRNG<double>(), UGenRNG<double>());
					sphereMat = std::make_shared<MLambertian>(matAlbedo);
					m_mainWorld.addWorldObj(std::make_unique<WOSphere>(center, 0.2, sphereMat));
				}
				else if (matChosen < 0.95) {
					// metal
					auto matAlbedo = Color(UGenRNG<double>(0.5, 1), UGenRNG<double>(0.5, 1), UGenRNG<double>(0.5, 1));
					auto fuzz = UGenRNG<double>(0, 0.5);
					sphereMat = std::make_shared<MMetal>(matAlbedo, fuzz);
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

	auto material1 = std::make_shared<MDielectric>(1.5);
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(0, 1, 0), 1.0, material1));

	auto material2 = std::make_shared<MLambertian>(Color(0.4, 0.2, 0.1));
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(-4, 1, 0), 1.0, material2));

	auto material3 = std::make_shared<MMetal>(Color(0.7, 0.6, 0.5), 0.0);
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(4, 1, 0), 1.0, material3));
}

void Indus::initializeEngine()
{
	m_mainRenderer.setupRenderer(m_mainRenderImageProps.pixelResolutionObj, m_mainRenderImageProps.aspectRatioObj);
	m_mainRenderFramebuffer.resize(static_cast<long long>(m_mainRenderer.getRendererCameraProps().camImgPropsObj.pixelResolutionObj.widthInPixels * m_mainRenderer.getRendererCameraProps().camImgPropsObj.pixelResolutionObj.heightInPixels));
	m_mainRenderer.setThreadingMode(m_isMultithreaded);

	setGlobalCallbackFunctors();
	m_statsOverlay.setupOverlay(m_isMultithreaded);
}

void Indus::setGlobalCallbackFunctors()
{
	setWindowFunctors();
	setRendererFunctors();
}

void Indus::runEngine()
{
	// Run window.
	Timer overlayTimer;
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
		m_mainRenderer.renderFrameMultiCore(m_mainRenderFramebuffer, m_mainWorld);
	} };

	const std::function<void()> renderSampleCollectionPassFunctor{[&]()
	{
		m_mainRenderer.samplesCollectionRenderPassMultiCore(m_mainWorld);
	} };

	const std::function<bool()> texUpdateCheckFunctor{ [&]()
	{
		return m_mainRenderer.checkForDrawUpdate();
	} };

	const std::function<std::vector<Color>()> mainRenderFramebufferGetter{ [&]()
	{
		return m_mainRenderFramebuffer;
	} };

	const std::function<CameraProperties()> mainRendererCamPropsGetter{ [&]()
	{
		return m_mainRenderer.getRendererCameraProps();
	} };
	
	const std::function<bool()> getRenderCompleteStatusGetter{ [&]()
	{
		return m_mainRenderer.getRenderCompleteStatus();
	} };

	const std::function<bool()> getSampleCollectionPassStatusGetter{ [&]()
	{
		return m_mainRenderer.getSampleCollectionPassCompleteStatus();
	} };

	const std::function<int()> textureUpdateRateGetter{ [&]()
	{
		return m_mainRenderer.getTexUpdateRate();
	} };

	m_mainWindow.setMainEngineFramebufferGetFunctor(mainRenderFramebufferGetter);
	m_mainWindow.setTextureUpdateCheckFunctor(texUpdateCheckFunctor);
	m_mainWindow.setMultithreadedCheckFunctor(multithreadingCheckFunctor);
	m_mainWindow.setRenderFrameMultiCoreFunctor(renderMultiCoreFrameFunctor);
	m_mainWindow.setMainRendererCameraPropsGetFunctor(mainRendererCamPropsGetter);
	m_mainWindow.setRenderCompleteStatusGetFunctor(getRenderCompleteStatusGetter);
	m_mainWindow.setTextureUpdateRateGetFunctor(textureUpdateRateGetter);
	m_mainWindow.setRenderSampleCollectionPassFunctor(renderSampleCollectionPassFunctor);
	m_mainWindow.setSampleCollectionPassCompleteStatusFunctor(getSampleCollectionPassStatusGetter);
}

std::vector<Color> Indus::getMainRenderFramebuffer() const noexcept
{
	return m_mainRenderFramebuffer;
}

