import esw_sfmlwindow;

import <vector>;
import <windows.h>;
import <string>;
import <memory>;
import <Pdh.h>;
import <any>;



void ESSFMLWindow::startPDHQuery(PDHVariables& pdhVars)
{
	std::string CPUStr{ "\\Processor(_Total)\\% Processor Time" };
	setupPDHQueryAndCounter(pdhVars.totalCPUUsage, std::wstring{ CPUStr.begin(), CPUStr.end() });
}

void ESSFMLWindow::stopPDHQuery(PDHVariables& pdhVars)
{
	PdhCloseQuery(pdhVars.totalCPUUsage.pdhQueryObj);
}


ESSFMLWindow& ESSFMLWindow::getSingletonInstance()
{
	static ESSFMLWindow singularWindowInstance{};
	return singularWindowInstance;
}

void ESSFMLWindow::initializeEntity()
{
	m_propertiesObj.sfmlAPIObj.renderWindowObj.create(sf::VideoMode(static_cast<int>(m_propertiesObj.windowResolution.widthInPixels * 0.5), static_cast<int>(m_propertiesObj.windowResolution.heightInPixels * 0.5)), m_propertiesObj.windowTitle);

	m_propertiesObj.sfmlAPIObj.mainRenderViewObj = sf::View(sf::FloatRect(0, 0, static_cast<float>(m_propertiesObj.windowResolution.widthInPixels), static_cast<float>(m_propertiesObj.windowResolution.heightInPixels)));
	m_propertiesObj.sfmlAPIObj.renderWindowObj.setView(m_propertiesObj.sfmlAPIObj.mainRenderViewObj);

	m_propertiesObj.sfmlAPIObj.renderWindowObj.setFramerateLimit(m_propertiesObj.windowFPSIntegral);

	m_propertiesObj.sfmlAPIObj.mainRenderTexObj.create(m_propertiesObj.windowResolution.widthInPixels, m_propertiesObj.windowResolution.heightInPixels);
	m_propertiesObj.sfmlAPIObj.mainRenderSpriteObj.setTexture(m_propertiesObj.sfmlAPIObj.mainRenderTexObj);

	m_propertiesObj.sfmlAPIObj.mainOverlayViewObj = sf::View(sf::FloatRect(0, 0, static_cast<float>(m_propertiesObj.windowResolution.widthInPixels), static_cast<float>(m_propertiesObj.windowResolution.heightInPixels)));
}

void ESSFMLWindow::startEntity()
{
	double totalDRAM{ retrieveTotalDRAM() };
	std::vector<std::any> processInputFuncArgs{};
	std::vector<std::any> updateWindowFuncArgs{};
	std::vector<std::any> drawGUIFuncArgs{};


	//statsOverlayObj.setTotalDRAM(totalDRAM);
	startPDHQuery(m_propertiesObj.pdhVars);
	m_propertiesObj.pdhTimer->startEntity();

	while (m_propertiesObj.sfmlAPIObj.renderWindowObj.isOpen())
	{
		processInputEvents(processInputFuncArgs);
		updateWindow(updateWindowFuncArgs);
		drawGUI(drawGUIFuncArgs);
	}

	PdhCloseQuery(m_propertiesObj.pdhVars.totalCPUUsage.pdhQueryObj);
}

void ESSFMLWindow::stopEntity()
{
	m_propertiesObj.sfmlAPIObj.renderWindowObj.close();
}

void ESSFMLWindow::updateWindow(const std::vector<std::any>& args)
{
	// Update texture for display.
	// Update PDH stats for overlay.
	// Update rendering status.
}

void ESSFMLWindow::processInputEvents(const std::vector<std::any>& args)
{
	sf::Event event{};
	while (m_propertiesObj.sfmlAPIObj.renderWindowObj.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			stopEntity();

		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::BackSpace)
			{
				//statsOverlayObj.setOverlayVisibility(!statsOverlayObj.getOverlayVisibility());
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::Space)
			{
				m_isRendering = true;
				//statsOverlayObj.setRenderingStartStatus(m_isRendering);
				//m_mainRenderSchedulerFuture = std::async(std::launch::async, m_windowFunctors.renderFrameMultiCoreFunctor);
				//m_needsDrawUpdate = true;
			}
		}
	}
}

void ESSFMLWindow::drawGUI(const std::vector<std::any>& args)
{

}

bool ESSFMLWindow::retrievePDHQueryValues(PDHVariables& pdhVars)
{
	if (m_propertiesObj.pdhTimer->getElapsedTime().count() > 1.5)
	{
		getFormattedValue(pdhVars);
		m_propertiesObj.pdhTimer->resetTimer();
		m_propertiesObj.pdhTimer->startEntity();
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

void ESSFMLWindow::getFormattedValue(PDHVariables& pdhQueryCounterObjVec)
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
