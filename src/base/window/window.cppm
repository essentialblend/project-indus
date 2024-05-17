import window;

import <windows.h>;
import <string>;
import <memory>;
import <Pdh.h>;

import color_rgb;

import <SFML/Graphics.hpp>;

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
					m_isRendering = true;
					statsOverlayObj.setRenderingStartStatus(m_isRendering);
					m_mainRenderSchedulerFuture = std::async(std::launch::async, m_windowFunctors.renderFrameMultiCoreFunctor);
					m_needsDrawUpdate = true;
				}
			}
		}
	}
}

void SFMLWindow::displayWindow(StatsOverlay& statsOverlayObj, Timer& timerObj)
{
	double totalDRAM{ retrieveTotalDRAM() };
	statsOverlayObj.setTotalDRAM(totalDRAM);

	startPDHQuery(m_pdhVars);
	setupWindowSFMLParams();

	m_cpuUsagePDHTimer.startTimer();
	while (m_windowProps.renderWindowObj.isOpen())
	{
		processInputEvents(statsOverlayObj, timerObj);
   		checkForUpdates(statsOverlayObj, timerObj, m_pdhVars, totalDRAM);
		drawGUI(statsOverlayObj, timerObj);
	}

	PdhCloseQuery(m_pdhVars.totalCPUUsage.pdhQueryObj);
}

void SFMLWindow::startPDHQuery(PDHVariables& pdhVars)
{
	std::string CPUStr{ "\\Processor(_Total)\\% Processor Time" };
	setupPDHQueryAndCounter(pdhVars.totalCPUUsage, std::wstring{CPUStr.begin(), CPUStr.end()});
}

void SFMLWindow::checkForUpdates(StatsOverlay& statsOverlayObj, Timer& timerObj, PDHVariables& pdhVars, double totalDRAMGigabytes)
{
	updateTextureForDisplay();
	updatePDHOverlayPeriodic(statsOverlayObj, pdhVars, totalDRAMGigabytes);
	updateRenderingStatus(timerObj, statsOverlayObj);
}

void SFMLWindow::updatePDHOverlayPeriodic(StatsOverlay& statsOverlayObj, PDHVariables& pdhVars, double totalDRAMGigabytes)
{
	if (retrievePDHQueryValues(pdhVars))
	{
		statsOverlayObj.setTotalCPUUsage(pdhVars.totalCPUUsage.pdhFmtCounterValObj.doubleValue);

		MEMORYSTATUSEX memInfo{};
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		DWORDLONG availPhysMem = memInfo.ullAvailPhys;
		double usedDRAMGigabytes{ totalDRAMGigabytes - (static_cast<double>(availPhysMem) / (1024.0 * 1024.0 * 1024.0)) };

		statsOverlayObj.setUsedDRAM(usedDRAMGigabytes);
	}
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
	//if (m_needsDrawUpdate && m_windowFunctors.isTextureReadyForUpdateFunctor())
	if(m_needsDrawUpdate && m_windowFunctors.isTextureReadyForUpdateFunctor())
	{
		displayWithSequentialTexUpdates();
	}
}

void SFMLWindow::displayWithSequentialTexUpdates()
{
	static int itStartOffset{ 0 };
	static int chunkTracker{ 0 };
	std::vector<sf::Uint8> localSFMLBuffer{};

	const auto& weightsVec{ m_windowFunctors.getGaussianKernelPropsFunctor().kernelWeights };
	const auto& localFramebuffer = m_windowFunctors.getMainEngineFramebufferFunctor();
	const auto localPixelRes = m_windowFunctors.getRendererCameraPropsFunctor().camImgPropsObj.pixelResolutionObj;
	const auto localTexUpdateRate = m_windowFunctors.getTextureUpdateRateFunctor();
	static int originalTexUpdateRate{ localTexUpdateRate };
	const int numPixelsInUpdateChunk { localPixelRes.widthInPixels * localTexUpdateRate };
	const auto& localColorType{ m_windowFunctors.getRenderColorTypeFunctor() };

	const auto itBegin{ localFramebuffer.begin() + static_cast<long long>(chunkTracker * (originalTexUpdateRate * localPixelRes.widthInPixels)) };
	const auto itEnd{ (localFramebuffer.end() - itBegin > numPixelsInUpdateChunk) ? itBegin + numPixelsInUpdateChunk : localFramebuffer.end() };
	localSFMLBuffer.reserve(static_cast<long long>(localTexUpdateRate * localPixelRes.widthInPixels * 4));

	for (auto it{ itBegin }; it != itEnd; ++it) {
		
		auto& colorObj{ *it };
		const auto index{ std::distance(localFramebuffer.begin(), it) };
		const double gaussianAccumWeight{ weightsVec[index] };

		colorObj->applyWeights(gaussianAccumWeight);
		colorObj->applyGammaCorrection(2.4);
		colorObj->undoNormalization(256);

		if (localColorType == "ColorRGB")
		{
			const auto& localColorObj = static_cast<const ColorRGB*>(colorObj.get());
			localSFMLBuffer.push_back(static_cast<sf::Uint8>(localColorObj->getRedComponent()));
			localSFMLBuffer.push_back(static_cast<sf::Uint8>(localColorObj->getGreenComponent()));
			localSFMLBuffer.push_back(static_cast<sf::Uint8>(localColorObj->getBlueComponent()));
			localSFMLBuffer.push_back(255);
		}
	}

	m_windowProps.mainRenderTexObj.update(localSFMLBuffer.data(), localPixelRes.widthInPixels, localTexUpdateRate, 0, chunkTracker * originalTexUpdateRate);

	++chunkTracker; itStartOffset += originalTexUpdateRate * localPixelRes.widthInPixels;

	if(itStartOffset > localPixelRes.widthInPixels * localPixelRes.heightInPixels)
	{
		itStartOffset = 0;
		chunkTracker = 0;
		m_needsDrawUpdate = false;
	}
}

void SFMLWindow::setupWindowSFMLParams()
{
	m_windowProps.renderWindowObj.create(sf::VideoMode(static_cast<int>(m_windowPixelRes.widthInPixels * m_windowProps.windowedResScale), static_cast<int>(m_windowPixelRes.heightInPixels * m_windowProps.windowedResScale)) , m_windowTitle);

	m_windowProps.mainRenderViewObj = sf::View(sf::FloatRect(0, 0, static_cast<float>(m_windowFunctors.getRendererCameraPropsFunctor().camImgPropsObj.pixelResolutionObj.widthInPixels), static_cast<float>(m_windowFunctors.getRendererCameraPropsFunctor().camImgPropsObj.pixelResolutionObj.heightInPixels)));
	m_windowProps.renderWindowObj.setView(m_windowProps.mainRenderViewObj);
	
	m_windowProps.renderWindowObj.setFramerateLimit(m_windowProps.prefFPSInIntegral);

	m_windowProps.mainRenderTexObj.create(m_windowFunctors.getRendererCameraPropsFunctor().camImgPropsObj.pixelResolutionObj.widthInPixels, m_windowFunctors.getRendererCameraPropsFunctor().camImgPropsObj.pixelResolutionObj.heightInPixels);
	m_windowProps.mainRenderSpriteObj.setTexture(m_windowProps.mainRenderTexObj);

	m_windowProps.mainOverlayViewObj = sf::View(sf::FloatRect(0, 0, static_cast<float>(m_windowPixelRes.widthInPixels), static_cast<float>(m_windowPixelRes.heightInPixels)));

}

void SFMLWindow::drawGUI(StatsOverlay& statsOverlayObj, const Timer& timerObj)
{
	m_windowProps.renderWindowObj.clear();

	m_windowProps.renderWindowObj.setView(m_windowProps.mainRenderViewObj);
	m_windowProps.renderWindowObj.draw(m_windowProps.mainRenderSpriteObj);
	
	m_windowProps.renderWindowObj.setView(m_windowProps.mainOverlayViewObj);
	statsOverlayObj.showOverlay(m_windowProps.renderWindowObj, m_windowPixelRes, timerObj);
	
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

void SFMLWindow::setMainEngineFramebufferGetFunctor(const std::function<std::vector<std::unique_ptr<IColor>>&()>&  mainEngineFramebufferGetFunctor) noexcept
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

void SFMLWindow::setGaussianKernelPropsGetFunctor(const std::function<GaussianKernelProperties()>& gaussianKernelPropsFunctor) noexcept
{
	m_windowFunctors.getGaussianKernelPropsFunctor = gaussianKernelPropsFunctor;
}

void SFMLWindow::setRenderColorTypeGetFunctor(const std::function<std::string()>& renderColorTypeFunctor) noexcept
{
	m_windowFunctors.getRenderColorTypeFunctor = renderColorTypeFunctor;
}

bool SFMLWindow::retrievePDHQueryValues(PDHVariables& pdhVars)
{
	if (m_cpuUsagePDHTimer.getElapsedTime().count() > 1.5)
	{
		getFormattedValue(pdhVars);
		m_cpuUsagePDHTimer.resetTimer();
		m_cpuUsagePDHTimer.startTimer();
		return true;
	}
	return false;
}

void SFMLWindow::getFormattedValue(PDHVariables& pdhVars)
{
	auto localCopy{ pdhVars.getPDHObjects() };

	for (auto& queryCounter : localCopy)
	{
		PDHQueryCounterVars& counterVars = queryCounter.get();

		if (PdhCollectQueryData(counterVars.pdhQueryObj) != ERROR_SUCCESS)
		{
			PdhCloseQuery(counterVars.pdhQueryObj);
			return;
		}

		PDH_STATUS pdhStatus = PdhGetFormattedCounterValue(counterVars.pdhCounterObj, PDH_FMT_DOUBLE, NULL, &counterVars.pdhFmtCounterValObj);
		if (pdhStatus != ERROR_SUCCESS)
		{
			PdhCloseQuery(counterVars.pdhQueryObj);
			return;
		}
	}
}

void SFMLWindow::setupPDHQueryAndCounter(PDHQueryCounterVars& pdhQueryAndCounter, const std::wstring& queryAPIString)
{
	if (PdhOpenQuery(NULL, 0, &pdhQueryAndCounter.pdhQueryObj) != ERROR_SUCCESS)
	{
		PdhCloseQuery(pdhQueryAndCounter.pdhQueryObj);
	}

	if (PdhAddCounter(pdhQueryAndCounter.pdhQueryObj, queryAPIString.c_str(), 0, &pdhQueryAndCounter.pdhCounterObj) != ERROR_SUCCESS)
	{
		PdhCloseQuery(pdhQueryAndCounter.pdhQueryObj);
	}

	if (PdhCollectQueryData(pdhQueryAndCounter.pdhQueryObj) != ERROR_SUCCESS)
	{
		PdhCloseQuery(pdhQueryAndCounter.pdhQueryObj);
	}
} 

double SFMLWindow::retrieveTotalDRAM()
{
	MEMORYSTATUSEX memInfo{};
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	return static_cast<double>(totalPhysMem / static_cast<double>(1024 * 1024 * 1024));	
}
