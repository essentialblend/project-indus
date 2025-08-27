import window;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../dep/stb_image_write.h"

#include <filesystem>

import core_color_util;

import <windows.h>;
import <string>;
import <memory>;
import <Pdh.h>;
import <cassert>;

void SFMLWindow::processInputEvents(StatsOverlay& statsOverlayObj, UTimer& timerObj)
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
			if (event.key.code == sf::Keyboard::Space && !m_isRendering)
			{
				timerObj.startTimer();
				m_isRendering = true;
				statsOverlayObj.setRenderingStartStatus(m_isRendering);
				
				m_mainRenderSchedulerFuture = std::async(std::launch::async, m_windowFunctors.renderFrameFunctor);

				m_needsDrawUpdate = true;
			}
			if (event.key.code == sf::Keyboard::T)
			{
				// Analytic Li Test
				// m_windowFunctors.liTestRenderFrameFunctor();
			}
		}
	}
}

void SFMLWindow::displayWindow(StatsOverlay& statsOverlayObj, UTimer& timerObj)
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

void SFMLWindow::checkForUpdates(StatsOverlay& statsOverlayObj, UTimer& timerObj, PDHVariables& pdhVars, double totalDRAMGigabytes)
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

void SFMLWindow::updateRenderingStatus(UTimer& timerObj, StatsOverlay& statsOverlayObj)
{
	if (m_isRendering && m_windowFunctors.getRenderCompleteStatusFunctor())
	{
		const auto localCamPixelImageObj { m_windowFunctors.getRendererCameraPropsFunctor().camImgPropsObj.pixelResolutionObj };

		const auto resWidth{ localCamPixelImageObj.widthInPixels };
		const auto resHeight{ localCamPixelImageObj.heightInPixels };

		timerObj.stopTimer();
		statsOverlayObj.setRenderingCompleteStatus(true);
		m_isRendering = false;

		saveFramebufferPNG(convertToRGBA8(m_windowFunctors.getMainEngineFramebufferFunctor(), resWidth, resHeight), resWidth, resHeight, static_cast<int>(m_renderSPP));
	}
}

void SFMLWindow::updateTextureForDisplay()
{
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

	const auto itBegin{ localFramebuffer.begin() + static_cast<long long>(chunkTracker * (originalTexUpdateRate * localPixelRes.widthInPixels)) };
	const auto itEnd{ (localFramebuffer.end() - itBegin > numPixelsInUpdateChunk) ? itBegin + numPixelsInUpdateChunk : localFramebuffer.end() };
	localSFMLBuffer.reserve(static_cast<long long>(localTexUpdateRate * localPixelRes.widthInPixels * 4));

	for (auto it{ itBegin }; it != itEnd; ++it) 
	{
		
		ColorRGB color{ *it };
		const auto index{ std::distance(localFramebuffer.begin(), it) };
	
		// Gamma and clamp, clamp via toSFMLColor
		color = applyGamma(color, 2.2);
		sf::Color sfmlColor{ toSFMLColor(color) };

		localSFMLBuffer.push_back(sfmlColor.r);
		localSFMLBuffer.push_back(sfmlColor.g);
		localSFMLBuffer.push_back(sfmlColor.b);
		localSFMLBuffer.push_back(sfmlColor.a);
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

	const auto imageWidthPixels{ m_windowFunctors.getRendererCameraPropsFunctor().camImgPropsObj.pixelResolutionObj.widthInPixels };
	const auto imageHeightPixels{ m_windowFunctors.getRendererCameraPropsFunctor().camImgPropsObj.pixelResolutionObj.heightInPixels };

	// Create main window to render in
	m_windowProps.renderWindowObj.create(sf::VideoMode(static_cast<int>(m_windowPixelRes.widthInPixels * m_windowProps.windowedResScale), static_cast<int>(m_windowPixelRes.heightInPixels * m_windowProps.windowedResScale)) , m_windowTitle);


	// Create the main viewport for the render window and set it as the main viewport
	m_windowProps.mainRenderViewObj = sf::View(sf::FloatRect(0, 0, static_cast<float>(imageWidthPixels), static_cast<float>(imageHeightPixels)));
	m_windowProps.renderWindowObj.setView(m_windowProps.mainRenderViewObj);
	
	m_windowProps.renderWindowObj.setFramerateLimit(m_windowProps.prefFPSInIntegral);

	// Set render texture and assign it to a sprite
	m_windowProps.mainRenderTexObj.create(imageWidthPixels, imageHeightPixels);
	m_windowProps.mainRenderSpriteObj.setTexture(m_windowProps.mainRenderTexObj);

	// Viewport for the overlay
	m_windowProps.mainOverlayViewObj = sf::View(sf::FloatRect(0, 0, static_cast<float>(m_windowPixelRes.widthInPixels), static_cast<float>(m_windowPixelRes.heightInPixels)));
}

void SFMLWindow::drawGUI(StatsOverlay& statsOverlayObj, const UTimer& timerObj)
{
	m_windowProps.renderWindowObj.clear();

	m_windowProps.renderWindowObj.setView(m_windowProps.mainRenderViewObj);
	m_windowProps.renderWindowObj.draw(m_windowProps.mainRenderSpriteObj);
	
	m_windowProps.renderWindowObj.setView(m_windowProps.mainOverlayViewObj);
	statsOverlayObj.showOverlay(m_windowProps.renderWindowObj, m_windowPixelRes, timerObj);
	
	m_windowProps.renderWindowObj.display();
}

SFMLWindowPropertiesOLD& SFMLWindow::getSFMLWindowProperties() noexcept
{
	return m_windowProps;
}

void SFMLWindow::setResolution(const PixelResolution& windowPixResObj) noexcept
{
	m_windowPixelRes = windowPixResObj;
}

void SFMLWindow::setRenderFrameFunctor(const std::function<void()>& renderFrameFunctor) noexcept
{
	m_windowFunctors.renderFrameFunctor = renderFrameFunctor;
}

void SFMLWindow::setLiTestRenderFrameFunctor(const std::function<void()>& liTestRenderFrameFunctor) noexcept
{
	m_windowFunctors.liTestRenderFrameFunctor = liTestRenderFrameFunctor;
}

void SFMLWindow::setMultithreadedCheckFunctor(const std::function<bool()>& isMultithreadedCheckFunctor) noexcept
{
	m_windowFunctors.isMultithreadedFunctor = isMultithreadedCheckFunctor;
}

void SFMLWindow::setTextureUpdateCheckFunctor(const std::function<bool()>& texUpdateCheckFunctor) noexcept
{
	m_windowFunctors.isTextureReadyForUpdateFunctor = texUpdateCheckFunctor;
}

void SFMLWindow::setMainEngineFramebufferGetFunctor(const std::function<std::vector<ColorRGB>&()>&  mainEngineFramebufferGetFunctor) noexcept
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

void SFMLWindow::setRenderSPP(std::size_t spp) noexcept
{
	m_renderSPP = spp;
}

void SFMLWindow::setSaveRenderImageStatus(bool isRenderSavedToDisk) noexcept
{
	m_shouldSaveRenderToDisk = isRenderSavedToDisk;
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

void SFMLWindow::saveFramebufferPNG(const std::vector<std::uint8_t>& framebuffer, int width, int height, int spp) const
{
	auto now{ std::chrono::system_clock::now() };
	std::string imageFilename{ std::format("render_{:%Y%m%d_%H%M%S}_{}x{}_{}spp.png",
		now, width, height, spp) };

	std::filesystem::path exeDir{ std::filesystem::current_path() / "renders" };
	std::filesystem::create_directories(exeDir);

	std::filesystem::path fullPath = exeDir / imageFilename;

	if (!stbi_write_png(fullPath.string().c_str(), width, height, 4,
		framebuffer.data(), width * 4)) {
		throw std::runtime_error("Failed to write PNG: " + fullPath.string());
	}
}
