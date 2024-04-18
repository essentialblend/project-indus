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
	const std::function<void()> renderFrameFunctor = [&]()
		{
			auto localProps = m_mainRenderer.getRendererCameraProps();
			m_mainRenderer.renderFrame(localProps.camImgPropsObj.pixelResolutionObj, localProps.camPixelDimObj, localProps.camCenter, m_mainRenderFramebuffer);
		};

	m_mainWindow.setRenderFrameFunctor(renderFrameFunctor);


	auto& mainDisplayTexObj{ m_mainWindow.getSFMLWindowProperties().texObj };
	const std::function<void(const sf::Uint8*, unsigned int, unsigned int, unsigned int, unsigned int)> drawTextureFunctor = [&mainDisplayTexObj](const sf::Uint8* pixelData, unsigned int widthRegionInPixels, unsigned int heightRegionInPixels, unsigned int xCoord, unsigned int yCoord)
		{
			mainDisplayTexObj.update(pixelData, widthRegionInPixels, heightRegionInPixels, xCoord, yCoord);
		};
	m_mainRenderer.setTextureUpdateFunctor(drawTextureFunctor);
}

void Indus::runEngine()
{
	// Run window.
	Timer t;
	m_mainWindow.displayWindow(m_statsOverlay, t);
}