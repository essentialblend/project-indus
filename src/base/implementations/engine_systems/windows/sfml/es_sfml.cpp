module es_sfml;

import es_timermanager;
import es_overlaymanager;
import eu_statsoverlay;

import <vector>;
import <windows.h>;
import <string>;
import <memory>;
import <algorithm>;
import <Pdh.h>;
import <any>;

void ESSFMLWindow::startPDHQuery()
{
	std::string CPUStr{ "\\Processor(_Total)\\% Processor Time" };
	setupPDHQueryAndCounter(m_propertiesObj.pdhVars.totalCPUUsage, std::wstring{ CPUStr.begin(), CPUStr.end() });
}

void ESSFMLWindow::stopPDHQuery()
{
	PdhCloseQuery(m_propertiesObj.pdhVars.totalCPUUsage.pdhQueryObj);
}

void ESSFMLWindow::initializeEntity([[maybe_unused]] const std::vector<std::any>& args)
{	
	m_propertiesObj.sfmlAPIObj.renderWindowObj.create(sf::VideoMode(static_cast<int>(m_propertiesObj.windowResolution.widthInPixels * 0.5), static_cast<int>(m_propertiesObj.windowResolution.heightInPixels * 0.5)), m_propertiesObj.windowTitle);
	m_propertiesObj.sfmlAPIObj.mainRenderViewObj = sf::View(sf::FloatRect(0, 0, static_cast<float>(m_propertiesObj.windowResolution.widthInPixels), static_cast<float>(m_propertiesObj.windowResolution.heightInPixels)));
	m_propertiesObj.sfmlAPIObj.renderWindowObj.setFramerateLimit(m_propertiesObj.windowFPSIntegral);
	m_propertiesObj.sfmlAPIObj.mainRenderTexObj.create(m_propertiesObj.windowResolution.widthInPixels, m_propertiesObj.windowResolution.heightInPixels);
	m_propertiesObj.sfmlAPIObj.mainRenderSpriteObj.setTexture(m_propertiesObj.sfmlAPIObj.mainRenderTexObj);
	m_propertiesObj.sfmlAPIObj.mainOverlayViewObj = sf::View(sf::FloatRect(0, 0, static_cast<float>(m_propertiesObj.windowResolution.widthInPixels), static_cast<float>(m_propertiesObj.windowResolution.heightInPixels)));


	if (!m_propertiesObj.sfmlAPIObj.idleScreenTextureObj.loadFromFile("src/dep/indus-alpha.png"))
	{
		throw std::runtime_error("Failed to load idle screen texture.");
	}
	m_propertiesObj.sfmlAPIObj.idleScreenSpriteObj.setTexture(m_propertiesObj.sfmlAPIObj.idleScreenTextureObj);
	m_propertiesObj.sfmlAPIObj.idleScreenTextureObj.setSmooth(true);
	m_propertiesObj.sfmlAPIObj.idleScreenSpriteObj.setScale(0.5f, 0.5f);

	auto engineSystemsLocal{ std::any_cast<EngineSystemTypes>(args[0]) };
	auto timerManager{ static_cast<ESTimerManager*>(engineSystemsLocal.masterTimerManagerObj) };
	auto overlayManager{ static_cast<ESOverlayManager*>(engineSystemsLocal.masterOverlayManagerObj) };

	for(auto& timer : timerManager->getTimersMutable())
	{
		if(timer->getTimerName() == "PDH Query Timer." || timer->getTimerName() == "Render Timer.")
		{
			m_timerList.push_back(timer);
		}
	}
	
	for(auto& overlay : overlayManager->getOverlaysMutable())
	{
		m_overlayList.push_back(overlay);
	}
}

void ESSFMLWindow::startEntity()
{
	m_propertiesObj.pdhResultVars.totalDRAMGB = retrieveTotalDRAM();
	std::for_each(m_timerList.begin(), m_timerList.end(), [](const std::shared_ptr<EUTimer>& timerPtr) { timerPtr->startEntity(); });
	
	startPDHQuery();

	while (m_propertiesObj.sfmlAPIObj.renderWindowObj.isOpen())
	{
		processInputEvents();
		updateWindow();
		drawWindowContent();
	}
}

void ESSFMLWindow::stopEntity()
{
	PdhCloseQuery(m_propertiesObj.pdhVars.totalCPUUsage.pdhQueryObj);
	m_propertiesObj.sfmlAPIObj.renderWindowObj.close();
}

void ESSFMLWindow::updateWindow([[maybe_unused]] const std::vector<std::any>& args)
{
	updateStatsOverlay();
}

void ESSFMLWindow::processInputEvents([[maybe_unused]] const std::vector<std::any>& args)
{
	sf::Event event{};
	while (m_propertiesObj.sfmlAPIObj.renderWindowObj.pollEvent(event))
	{
		if (event.type == sf::Event::Closed) stopEntity();

		if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::BackSpace)
			{
				for (auto& overlay : m_overlayList)
				{
					overlay->flipDisplayStatus();
				}
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::Space)
			{
				m_isRendering = true;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			if(event.key.code == sf::Keyboard::Escape)
			{
				stopEntity();
			}
		}
	}
}

void ESSFMLWindow::drawWindowContent([[maybe_unused]] const std::vector<std::any>& args)
{
	std::vector<std::any> overlayArgs{};
	auto& renderWindowObj{ m_propertiesObj.sfmlAPIObj.renderWindowObj };
	
	renderWindowObj.clear();
	renderWindowObj.setView(m_propertiesObj.sfmlAPIObj.mainRenderViewObj);

	if (m_isRendering == false)
	{
		drawIdleScreen();
	}
	else
	{
		// Draw render texture.
		renderWindowObj.draw(m_propertiesObj.sfmlAPIObj.mainRenderSpriteObj);
	}

	// Draw overlay. (TBD dynamic fill-rect calculation.)
	renderWindowObj.setView(m_propertiesObj.sfmlAPIObj.mainOverlayViewObj);
	overlayArgs.push_back(std::ref(renderWindowObj));
	for(auto& overlay : m_overlayList)
	{
		if(overlay->getOverlayVisibility())
			overlay->showOverlay(overlayArgs);
	}

	// Display drawn content.
	m_propertiesObj.sfmlAPIObj.renderWindowObj.display();
}

void ESSFMLWindow::updateStatsOverlay()
{
	if (retrievePDHQueryValues())
	{
		MEMORYSTATUSEX memInfo{};
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		DWORDLONG availPhysMem = memInfo.ullAvailPhys;
		double usedDRAMGigabytes{ m_propertiesObj.pdhResultVars.totalDRAMGB - (static_cast<double>(availPhysMem) / (1024.0 * 1024.0 * 1024.0)) };

		auto statsOverlayPtr{*(std::find_if(m_overlayList.begin(), m_overlayList.end(), [](const std::shared_ptr<EUOverlay>& overlayPtr) { return std::static_pointer_cast<EUStatsOverlay>(overlayPtr) != nullptr; }))};
		std::vector<std::any> statOverlayArgs{ m_propertiesObj.pdhVars.totalCPUUsage.pdhFmtCounterValObj.doubleValue, m_propertiesObj.pdhResultVars.totalDRAMGB, usedDRAMGigabytes };
		statsOverlayPtr->updateOverlayContent(statOverlayArgs);
	}
}

bool ESSFMLWindow::retrievePDHQueryValues()
{
	auto pdhQueryTimer{std::find_if(m_timerList.begin(), m_timerList.end(), 
		[](const std::shared_ptr<EUTimer>& timerPtr) 
		{ 
			return timerPtr->getTimerName() == "PDH Query Timer."; 
		})};

	if ((*pdhQueryTimer)->getElapsedTime().count() > 1.5)
	{
		getFormattedValue();
		(*pdhQueryTimer)->resetTimer();
		(*pdhQueryTimer)->startEntity();
		return true;
	}
	
	return false;
}

void ESSFMLWindow::setupPDHQueryAndCounter(PDHQueryCounterVars& pdhQueryAndCounter, const std::wstring& queryAPIString)
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

void ESSFMLWindow::getFormattedValue()
{
	auto localCopy{ m_propertiesObj.pdhVars.getPDHObjects() };

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

double ESSFMLWindow::retrieveTotalDRAM()
{
	MEMORYSTATUSEX memInfo{};
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	return static_cast<double>(totalPhysMem / static_cast<double>(1024 * 1024 * 1024));
}

void ESSFMLWindow::drawIdleScreen() 
{
	m_propertiesObj.sfmlAPIObj.renderWindowObj.draw(m_propertiesObj.sfmlAPIObj.idleScreenSpriteObj);
}


