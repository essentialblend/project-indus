import indus;

import color;
import sphere;
import material;
import lambertian;
import metal;
import dielectric;

Indus::Indus(const PixelResolution& windowPixResObj, const PixelResolution& imagePixResObj, const AspectRatio& aspectRatioObj) noexcept :
	m_mainWindow(windowPixResObj),
	m_mainRenderImageProps{
		.pixelResolutionObj{imagePixResObj},
		.aspectRatioObj {aspectRatioObj} } {};

void Indus::initializeWorld()
{
	const auto ground_lamb = std::make_shared<MLambertian>(Color(0.8, 0.8, 0.0));
	const auto center_lamb = std::make_shared<MLambertian>(Color(0.1, 0.2, 0.5));
	const auto left_diel = std::make_shared<MDielectric>(1.50);
	const auto bubble_diel = std::make_shared<MDielectric>(1.00 / 1.50);
	const auto right_metal = std::make_shared<MMetal>(Color(0.8, 0.6, 0.2));

	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(0.0, -100.5, -1.0), 100.0, ground_lamb));
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(0.0, 0.0, -1.2), 0.5, center_lamb));
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(-1.0, 0.0, -1.0), 0.5, left_diel));
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(-1.0, 0.0, -1.0), 0.4, bubble_diel));
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(1.0, 0.0, -1.0), 0.5, right_metal));
}

void Indus::initializeEngine()
{
	m_mainRenderer.setupRenderer(m_mainRenderImageProps.pixelResolutionObj, m_mainRenderImageProps.aspectRatioObj);
	m_mainRenderFramebuffer.resize(static_cast<long long>(m_mainRenderer.getRendererCameraProps().camImgPropsObj.pixelResolutionObj.widthInPixels * m_mainRenderer.getRendererCameraProps().camImgPropsObj.pixelResolutionObj.heightInPixels));
	m_mainRenderer.setThreadingMode(m_isMultithreaded);

	setGlobalCallbackFunctors();
	m_mainWindow.setupWindow();
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
	auto& mainDisplayTexObj{ m_mainWindow.getSFMLWindowProperties().texObj };

	localCopy.sfmlTextureUpdateFunctor = [&mainDisplayTexObj](const sf::Uint8* pixelData, unsigned int widthRegionInPixels, unsigned int heightRegionInPixels, unsigned int xCoord, unsigned int yCoord)
		{
			mainDisplayTexObj.update(pixelData, widthRegionInPixels, heightRegionInPixels, xCoord, yCoord);
		};
	localCopy.sfmlClearWindowFunctor = [this]() {m_mainWindow.getSFMLWindowProperties().renderWindowObj.clear(); };
	localCopy.sfmlDisplayWindowFunctor = [this]() {m_mainWindow.getSFMLWindowProperties().renderWindowObj.display(); };
	localCopy.sfmlDrawSpriteFunctor = [this]()
		{
			const auto& tempSprite = m_mainWindow.getSFMLWindowProperties().spriteObj;
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
	
	const std::function<bool()> mainRenderCompleteStatusGetter{ [&]()
	{
		return m_mainRenderer.getRenderCompleteStatus();
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
	m_mainWindow.setRenderCompleteStatusGetFunctor(mainRenderCompleteStatusGetter);
	m_mainWindow.setTextureUpdateRateGetFunctor(textureUpdateRateGetter);
}

std::vector<Color> Indus::getMainRenderFramebuffer() const noexcept
{
	return m_mainRenderFramebuffer;
}

