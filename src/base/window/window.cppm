import window;

import <chrono>;
import <algorithm>;
import <execution>;
import <iostream>;

import<windows.h>;
import<Pdh.h>;

import <SFML/Graphics.hpp>;

void SFMLWindow::setupWindow()
{
	const auto test = m_windowFunctors.getRendererCameraPropsFunctor();
	m_windowProps.viewObj = sf::View(sf::FloatRect(0, 0, static_cast<float>(test.camImgPropsObj.pixelResolutionObj.widthInPixels), static_cast<float>(test.camImgPropsObj.pixelResolutionObj.heightInPixels)));

	m_windowProps.texObj.create(test.camImgPropsObj.pixelResolutionObj.widthInPixels, test.camImgPropsObj.pixelResolutionObj.heightInPixels);
	m_windowProps.spriteObj = sf::Sprite();
}

void SFMLWindow::processInputEvents(StatsOverlay& statsOverlayObj, Timer& timerObj)
{
	sf::Event event{};
	while (m_windowProps.renderWindowObj.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_windowProps.renderWindowObj.close();

		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::BackSpace)
			{
				statsOverlayObj.setOverlayVisibility(!statsOverlayObj.getOverlayVisibility());
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::Space)
			{
				timerObj.startTimer();
				if (m_windowFunctors.isMultithreadedFunctor()) [[likely]]
				{
					m_renderingStatusFuture = std::async(std::launch::async, [&]()
					{
						m_needsDrawUpdate = true;
						m_isRendering = true;
						m_windowFunctors.renderFrameMultiCoreFunctor();
					});
				}
			}
		}
	}
}

void SFMLWindow::displayWindow(StatsOverlay& statsOverlayObj, Timer& timerObj)
{

	startPDHQuery(m_pdhVars);
	setupWindowSFMLParams();
	m_cpuUsagePDHTimer.startTimer();

	while (m_windowProps.renderWindowObj.isOpen())
	{
		processInputEvents(statsOverlayObj, timerObj);
   		checkForUpdates(statsOverlayObj, timerObj, m_pdhVars);
		drawGUI(statsOverlayObj, timerObj);
	}

	PdhCloseQuery(m_pdhVars.cpuQuery);
}

void SFMLWindow::startPDHQuery(PDHVariables& pdhVars)
{
	if (PdhOpenQuery(NULL, 0, &pdhVars.cpuQuery) != ERROR_SUCCESS)
	{
		PdhCloseQuery(pdhVars.cpuQuery);
	}

	if (PdhAddCounter(pdhVars.cpuQuery, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &pdhVars.cpuTotal) != ERROR_SUCCESS)
	{
		PdhCloseQuery(pdhVars.cpuQuery);
	}

	if (PdhCollectQueryData(pdhVars.cpuQuery) != ERROR_SUCCESS)
	{
		PdhCloseQuery(pdhVars.cpuQuery);
	}
}

void SFMLWindow::checkForUpdates(StatsOverlay& statsOverlayObj, Timer& timerObj, PDHVariables& pdhVars)
{
	updateTextureForDisplay();
	updatePDHOverlayPeriodic(statsOverlayObj, pdhVars);
	updateRenderingStatus(timerObj, statsOverlayObj);
}

void SFMLWindow::updatePDHOverlayPeriodic(StatsOverlay& statsOverlayObj, PDHVariables& pdhVars)
{
	statsOverlayObj.setTotalCPUUsage(getCPUUsageWithPDH(pdhVars));
}

void SFMLWindow::updateRenderingStatus(Timer& timerObj, StatsOverlay& statsOverlayObj)
{
	if (m_isRendering && m_windowFunctors.getRenderCompleteStatusFunctor())
	{
		timerObj.endTimer();
		statsOverlayObj.setRenderingCompleteStatus(true);
		m_isRendering = false;
	}
}

void SFMLWindow::updateTextureForDisplay()
{
	if (m_needsDrawUpdate && m_windowFunctors.isTextureReadyForUpdateFunctor())
	{
		// chunkTracker increments from 0 to (numRows - 1).
		static int chunkTracker{ 0 };
		const auto& localMainFramebuffer{ m_windowFunctors.getMainEngineFramebufferFunctor() };
		const auto& localImgPixResObj{ m_windowFunctors.getRendererCameraPropsFunctor().camImgPropsObj.pixelResolutionObj };
		int texUpdateRate{ m_windowFunctors.getTextureUpdateRateFunctor() };
		std::vector<sf::Uint8> localTexSFMLBuffer;
		int singleChunkPixels{ localImgPixResObj.widthInPixels * texUpdateRate };

		const auto itBegin{ localMainFramebuffer.begin() + (static_cast<long long>(chunkTracker * singleChunkPixels)) };
		const auto remainingPixels{ std::distance(itBegin, localMainFramebuffer.end()) };

		if (remainingPixels < singleChunkPixels)
		{
			singleChunkPixels = static_cast<int>(remainingPixels);
			texUpdateRate = static_cast<int>(remainingPixels / localImgPixResObj.widthInPixels);
		}

		const auto itEnd{ itBegin + singleChunkPixels };
		localTexSFMLBuffer.reserve(static_cast<long long>(singleChunkPixels * 4));

		std::for_each(itBegin, itEnd, [&](const Color& color)
			{
				localTexSFMLBuffer.push_back(static_cast<sf::Uint8>(color.convertFromNormalized().getBaseVec().getX()));
				localTexSFMLBuffer.push_back(static_cast<sf::Uint8>(color.convertFromNormalized().getBaseVec().getY()));
				localTexSFMLBuffer.push_back(static_cast<sf::Uint8>(color.convertFromNormalized().getBaseVec().getZ()));
				localTexSFMLBuffer.push_back(255);
			});

		m_windowProps.texObj.update(localTexSFMLBuffer.data(), localImgPixResObj.widthInPixels, texUpdateRate, 0, texUpdateRate * chunkTracker);

		if (itEnd == localMainFramebuffer.end())
		{
			m_needsDrawUpdate = false;
			chunkTracker = 0;
		}
		else
		{
			++chunkTracker;
		}
	}
}

void SFMLWindow::setupWindowSFMLParams()
{
	m_windowProps.renderWindowObj.create(sf::VideoMode(static_cast<int>(m_windowPixelRes.widthInPixels * m_windowProps.windowedResScale), static_cast<int>(m_windowPixelRes.heightInPixels * m_windowProps.windowedResScale)) , m_windowTitle);

	m_windowProps.renderWindowObj.setView(m_windowProps.viewObj);
	m_windowProps.renderWindowObj.setFramerateLimit(m_windowProps.prefFPSInIntegral);
	m_windowProps.texObj.create(m_windowPixelRes.widthInPixels, m_windowPixelRes.heightInPixels);
	m_windowProps.spriteObj.setTexture(m_windowProps.texObj);
}

void SFMLWindow::drawGUI(StatsOverlay& statsOverlayObj, const Timer& timerObj)
{
	m_windowProps.renderWindowObj.clear();
	m_windowProps.renderWindowObj.draw(m_windowProps.spriteObj);
	statsOverlayObj.showOverlay(m_windowProps.renderWindowObj, m_windowPixelRes , timerObj);
	m_windowProps.renderWindowObj.display();
}

SFMLWindowProperties& SFMLWindow::getSFMLWindowProperties() noexcept
{
	return m_windowProps;
}

void SFMLWindow::setResolution(const PixelResolution& windowPixResObj) noexcept
{
	m_windowPixelRes = windowPixResObj;
}

void SFMLWindow::setRenderFrameMultiCoreFunctor(const std::function<void()>& multiCoreFunctor) noexcept
{
	m_windowFunctors.renderFrameMultiCoreFunctor = multiCoreFunctor;
}

void SFMLWindow::setMultithreadedCheckFunctor(const std::function<bool()>& isMultithreadedCheckFunctor) noexcept
{
	m_windowFunctors.isMultithreadedFunctor = isMultithreadedCheckFunctor;
}

void SFMLWindow::setTextureUpdateCheckFunctor(const std::function<bool()>& texUpdateCheckFunctor) noexcept
{
	m_windowFunctors.isTextureReadyForUpdateFunctor = texUpdateCheckFunctor;
}

void SFMLWindow::setMainEngineFramebufferGetFunctor(const std::function<std::vector<Color>()>& mainEngineFramebufferGetFunctor) noexcept
{
	m_windowFunctors.getMainEngineFramebufferFunctor = mainEngineFramebufferGetFunctor;
}

void SFMLWindow::setMainRendererCameraPropsGetFunctor(const std::function<CameraProperties()>& mainRendererCameraPropsGetFunctor)
{
	m_windowFunctors.getRendererCameraPropsFunctor = mainRendererCameraPropsGetFunctor;
}

void SFMLWindow::setRenderCompleteStatusGetFunctor(const std::function<bool()>& renderCompleteStatusFunctor) noexcept
{
	m_windowFunctors.getRenderCompleteStatusFunctor = renderCompleteStatusFunctor;
}

void SFMLWindow::setTextureUpdateRateGetFunctor(const std::function<int()>& texUpdateRateFunctor) noexcept
{
	m_windowFunctors.getTextureUpdateRateFunctor = texUpdateRateFunctor;
}

double SFMLWindow::getCPUUsageWithPDH(PDHVariables& pdhVars)
{
	
	if (m_cpuUsagePDHTimer.getElapsedTime().count() > 1.5)
	{
		if (PdhCollectQueryData(pdhVars.cpuQuery) != ERROR_SUCCESS)
		{
			PdhCloseQuery(pdhVars.cpuQuery);
			return -1;
		}

		PDH_STATUS pdhStatus = PdhGetFormattedCounterValue(pdhVars.cpuTotal, PDH_FMT_DOUBLE, NULL, &pdhVars.counterValue);
		if (pdhStatus != ERROR_SUCCESS)
		{
			PdhCloseQuery(pdhVars.cpuQuery);
			return -1;
		}
		m_cpuUsagePDHTimer.resetTimer();
		m_cpuUsagePDHTimer.startTimer();
		return pdhVars.counterValue.doubleValue;
	}
	return pdhVars.counterValue.doubleValue;
}