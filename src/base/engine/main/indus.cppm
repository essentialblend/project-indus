import indus;

import <vector>;
import <span>;

import color;

void Indus::initializeEngine()
{
	m_mainWindow.setupWindow();
	m_statsOverlay.setupOverlay();
	m_mainRenderer.setupRenderer(m_mainRenderImageProps.pixelResolutionObj, m_mainRenderImageProps.aspectRatioObj);

	setGlobalFunctors();
}

void Indus::setGlobalFunctors()
{
	// Window functors.
	const std::function<void()> renderFrameFunctor = [&]()
		{
			auto localProps = m_mainRenderer.getRendererCameraProps();
			m_mainRenderer.renderFrame(localProps.camImgPropsObj.pixelResolutionObj, localProps.camPixelDimObj, localProps.camCenter, m_mainRenderFramebuffer);
		};

	m_mainWindow.setRenderFrameFunctor(renderFrameFunctor);


	// Renderer functors
	RendererFunctors localCopy{};
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