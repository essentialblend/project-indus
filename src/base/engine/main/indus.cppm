import indus;

import sphere;
import material;
import matte;
import core_util;
import fresnel;
import glass;
import core_diag;

Indus::Indus(const PixelResolution& windowPixResObj, const PixelResolution& imagePixResObj, const AspectRatio& aspectRatioObj, int SPP, int rayBounceDepth, bool isDiagOutputEnabled, bool isRenderSavedToDisk, bool isMultithreaded) noexcept : m_mainWindow(windowPixResObj),
m_mainRenderImageProps{ imagePixResObj, aspectRatioObj }, m_SPP{ isMultithreaded ? SPP : 1}, m_rayBounceDepth{ rayBounceDepth == 0 ? 10 : rayBounceDepth }, m_isMultithreaded{ isMultithreaded }
{
  diagRunFlag.store(isDiagOutputEnabled, std::memory_order_relaxed);
	m_mainWindow.setSaveRenderImageStatus(isRenderSavedToDisk);
};

void Indus::initializeWorld()
{
	renderShirleyFirstBookScene();
}

void Indus::initializeEngine()
{

	m_mainRenderer.setSPP(m_SPP);
	m_mainWindow.setRenderSPP(!m_isMultithreaded ? 1 : m_SPP);
	m_mainRenderer.setThreadingMode(m_isMultithreaded);

	m_mainRenderer.setRayBounceDepth(m_rayBounceDepth);
	m_mainRenderer.setupRenderer(m_mainRenderImageProps.pixelResolutionObj, m_mainRenderImageProps.aspectRatioObj);

	
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
	// Run window
	UTimer overlayTimer;

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

	const std::function<void()> renderFrameFunctor{ [&]()
	{
		m_mainRenderer.renderFrame(m_mainRenderFramebuffer, m_mainWorld, m_isMultithreaded);
	}};

	// Li Test: Temporary
	const std::function<void()> liTestRenderFrameFunctor{ [&]()
	{
		m_mainRenderer.renderLiTestFrame(m_mainRenderFramebuffer, m_LiTestWorld, m_isMultithreaded);
	} };
	

	const std::function<bool()> texUpdateCheckFunctor{ [&]()
	{
		return m_mainRenderer.checkForDrawUpdate();
	}};

	const std::function<std::vector<ColorRGB>&()> mainRenderFramebufferGetter{[&]()
	-> std::vector<ColorRGB>& {
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

	m_mainWindow.setMainEngineFramebufferGetFunctor(mainRenderFramebufferGetter);
	m_mainWindow.setTextureUpdateCheckFunctor(texUpdateCheckFunctor);
	m_mainWindow.setMultithreadedCheckFunctor(multithreadingCheckFunctor);
	m_mainWindow.setRenderFrameFunctor(renderFrameFunctor);
	m_mainWindow.setMainRendererCameraPropsGetFunctor(mainRendererCamPropsGetter);
	m_mainWindow.setRenderCompleteStatusGetFunctor(getRenderCompleteStatusGetter);
	m_mainWindow.setTextureUpdateRateGetFunctor(textureUpdateRateGetter);
	m_mainWindow.setGaussianKernelPropsGetFunctor(gaussianKernelPropsGetter);

	m_mainWindow.setLiTestRenderFrameFunctor(liTestRenderFrameFunctor);
}

void Indus::renderShirleyFirstBookScene()
{
	const auto groundAlbedo{ ColorRGB{ 0.5 } };
	auto groundMat = std::make_shared<MMatte>(groundAlbedo);
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(0, -1000, 0), 1000, groundMat));

	for (int a = -10; a < 10; a++) {
		for (int b = -10; b < 10; b++) {
			auto matChosen = UGenRNG<double>();
			Point center(a + 0.9 * UGenRNG<double>(), 0.2, b + 0.9 * UGenRNG<double>());

			if ((center - Point(4, 0.2, 0)).getMagnitude() > 0.9)
			{
				std::shared_ptr<IMaterial> sphereMat;

				if (matChosen < 0.8)
				{
					// diffuse
					const auto matAlbedo{ ColorRGB(UGenRNG<double>(), UGenRNG<double>(), UGenRNG<double>()) * ColorRGB(UGenRNG<double>(), UGenRNG<double>(), UGenRNG<double>()) };
					sphereMat = std::make_shared<MMatte>(matAlbedo);
					m_mainWorld.addWorldObj(std::make_unique<WOSphere>(center, 0.2, sphereMat));
				}
				//else if (matChosen < 0.95) {
				//	// metal
				//	const auto matAlbedo{ ColorRGB(UGenRNG<double>(0.5, 1), UGenRNG<double>(0.5, 1), UGenRNG<double>(0.5, 1)) };
				//	auto fuzz = UGenRNG<double>(0, 0.5);
				//	sphereMat = std::make_shared<MMetal>(matAlbedo, fuzz);
				//	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(center, 0.2, sphereMat));
				//}
				else {
					// glass
					//sphereMat = std::make_shared<MSpecularMirror>(ColorRGB{ 1.0, 1.0, 1.0 }, frMirror);
					//m_mainWorld.addWorldObj(std::make_unique<WOSphere>(center, 0.2, sphereMat));
				}
			}
		}
	}

	const auto firstMat = std::make_shared<MGlass>(ColorRGB{ 1.0, 1.0, 1.0 }, ColorRGB{ 1.0, 1.0, 1.0 }, 1.0, 1.45);
	m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(-1.3, 1.1, 0), 1.1, firstMat));

	//auto thirdMat = std::make_shared<MMetal>(ColorRGB(0.7, 0.6, 0.5), 0.0);
	//m_mainWorld.addWorldObj(std::make_unique<WOSphere>(Point(4, 1, 0), 1.0, thirdMat));
}

void Indus::renderFresnelTestScene() 
{
	// Materials
	auto glassMat = std::make_shared<MGlass>(
		ColorRGB{ 1.0 }, ColorRGB{ 1.0 }, 1.0, 1.5);
	auto groundMat = std::make_shared<MMatte>(
		ColorRGB{ 0.05, 0.05, 0.05 });
	auto redMat = std::make_shared<MMatte>(
		ColorRGB{ 0.9, 0.1, 0.1 });
	auto yellowMat = std::make_shared<MMatte>(
		ColorRGB{ 0.9, 0.9, 0.1 });

	// Ground
	m_mainWorld.addWorldObj(
		std::make_unique<WOSphere>(Point(0, -1000, 0), 1000, groundMat));

	// Camera frame
	auto camProps = m_mainRenderer.getRendererCameraProps();
	Vec3 camDir = -camProps.camW;
	Vec3 camRight = camProps.camU;
	Vec3 camUp = camProps.camV;

	// Glass sphere at origin, sitting on ground
	Point glassCenter{ 0,1,0 };
	m_mainWorld.addWorldObj(
		std::make_unique<WOSphere>(glassCenter, 1.0, glassMat));

	// Red ball behind glass, directly along view axis
	m_mainWorld.addWorldObj(
		std::make_unique<WOSphere>(glassCenter + camDir * 3.0, 0.5, redMat));

	// Yellow wall sphere to the right & up, reflection target
	m_mainWorld.addWorldObj(
		std::make_unique<WOSphere>(glassCenter + camRight * 5.0 + camUp * 2.0, 3.0, yellowMat));
}

void Indus::renderLiTestScene()
{
	const auto groundAlbedo{ ColorRGB{ 0.5 } };
	auto groundMat = std::make_shared<MMatte>(groundAlbedo);

	auto camDir = -m_mainRenderer.getRendererCameraProps().camW;
	auto camPos = m_mainRenderer.getRendererCameraProps().camLookFrom;

	Point sphereCenter = camPos + camDir * 5.0;
	m_LiTestWorld.addWorldObj(std::make_unique<WOSphere>(sphereCenter, 1.0, groundMat));
	m_LiTestWorld.addWorldObj(std::make_unique<WOSphere>(Point(0, 0, 0), 0.5, groundMat));
}

std::vector<ColorRGB>& Indus::getMainRenderFramebuffer() noexcept
{
	return m_mainRenderFramebuffer;
}

