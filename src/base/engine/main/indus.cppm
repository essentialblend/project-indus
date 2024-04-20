import indus;

import <vector>;
import <span>;

import color;

Indus::Indus(const PixelResolution& windowPixResObj, const PixelResolution& imagePixResObj, const AspectRatio& aspectRatioObj) noexcept :
	m_mainWindow(windowPixResObj),
	m_mainRenderImageProps{
		.pixelResolutionObj{imagePixResObj},
		.aspectRatioObj {aspectRatioObj} } {};

void Indus::initializeEngine()
{
	m_mainWindow.setupWindow();
	m_statsOverlay.setupOverlay(m_isMultithreaded);
	m_mainRenderer.setupRenderer(m_mainRenderImageProps.pixelResolutionObj, m_mainRenderImageProps.aspectRatioObj);
	m_mainRenderFramebuffer.resize(static_cast<long long>(m_mainRenderer.getRendererCameraProps().camImgPropsObj.pixelResolutionObj.widthInPixels * m_mainRenderer.getRendererCameraProps().camImgPropsObj.pixelResolutionObj.heightInPixels));
	m_mainRenderer.setThreadingMode(m_isMultithreaded);

	setGlobalCallbackFunctors();
}

void Indus::setGlobalCallbackFunctors()
{
	// Window functors.
	const std::function<bool()> multithreadingCheckFunctor = [&]() { return m_mainRenderer.getThreadingMode(); };

	const std::function<void()> renderSingleCoreFrameFunctor = [&]()
		{
			auto localProps = m_mainRenderer.getRendererCameraProps();
			m_mainRenderer.renderFrameSingleCore(localProps.camImgPropsObj.pixelResolutionObj, localProps.camPixelDimObj, localProps.camCenter, m_mainRenderFramebuffer);
		};

	const std::function<void()> renderMultiCoreFrameFunctor = [&]()
		{
			auto localProps = m_mainRenderer.getRendererCameraProps();
			m_mainRenderer.renderFrameMultiCore(localProps.camImgPropsObj.pixelResolutionObj, localProps.camPixelDimObj, localProps.camCenter, m_mainRenderFramebuffer);
		};
	
	m_mainWindow.setMultithreadedCheckFunctor(multithreadingCheckFunctor);
	m_mainWindow.setRenderFrameSingleCoreFunctor(renderSingleCoreFrameFunctor);
	m_mainWindow.setRenderFrameMultiCoreFunctor(renderMultiCoreFrameFunctor);

	// Renderer functors
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

	m_mainRenderer.setRendererFunctors(localCopy);
}

void Indus::runEngine()
{
	// Run window.
	Timer t;
	m_mainWindow.displayWindow(m_statsOverlay, t);
}