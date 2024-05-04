import stats_overlay;

import <iostream>;
import <iomanip>;
import <sstream>;

void StatsOverlay::setupOverlay(bool isMultithreaded)
{
	if (!m_overlayFont.loadFromFile("src/dep/CourierPrime-Regular.ttf"))
	{
		m_isOverlayEnabled = false;
		return;
	}

	setSFMLTextProperties();

	setDefaultDisplayedText(isMultithreaded);
}

void StatsOverlay::setDefaultDisplayedText(bool isMultithreaded)
{
	std::vector<std::string> leftSideTitles{ "indus:", "Mode:", "Time:", "Status:", "CPU Usage:" };
	std::vector<std::string> rightSideValues{ "v0.0.0-alpha.", (isMultithreaded ? "Multithreaded." : "Singlethreaded."), "Awaiting render.", "Idle.", "N.A." };
	auto localOverlayProps{ m_overlayProps.getStatsCollection() };

	for (std::size_t i{ 0 }; i < leftSideTitles.size(); ++i)
	{
		localOverlayProps[i].get().statTitleObj.setString(leftSideTitles[i]);
		localOverlayProps[i].get().statResultObj.setString(rightSideValues[i]);
	}
}

void StatsOverlay::setSFMLTextProperties()
{
	std::vector<std::reference_wrapper<OverlayStatistic>> localStatsCollection{ m_overlayProps.getStatsCollection() };

	for (auto& overlayStat : localStatsCollection)
	{
		overlayStat.get().statTitleObj.setFont(m_overlayFont);
		overlayStat.get().statTitleObj.setCharacterSize(22);
		overlayStat.get().statTitleObj.setColor(sf::Color::White);

		overlayStat.get().statResultObj.setFont(m_overlayFont);
		overlayStat.get().statResultObj.setCharacterSize(22);
		overlayStat.get().statResultObj.setColor(sf::Color::White);
	}
}

void StatsOverlay::showOverlay(sf::RenderWindow& renderWindowObj, const PixelResolution& pixResObj, const Timer& timerObj)
{
	if (!m_isOverlayEnabled)
	{
		return;
	}

	std::ostringstream osStream;
	osStream << std::fixed << std::setprecision(2) << m_totalCPUUsage;
	std::string formattedCPUUsageStr{ osStream.str() + "%." };
	m_overlayProps.cpuUsageObj.statResultObj.setString(formattedCPUUsageStr);
	
	const float resultTextStartPos{ static_cast<float>(pixResObj.widthInPixels) };


	int verticalStatSpacing{ 0 };
	std::vector<std::reference_wrapper<OverlayStatistic>> tempCopy{ m_overlayProps.getStatsCollection() };

	if (m_hasRenderStarted && !m_hasPixelSampleCollectionCompleted)
	{
		m_overlayProps.renderObj.statResultObj.setString("I - Sample gen.");
		m_overlayProps.renderObj.statResultObj.setColor(sf::Color::Red);
		m_overlayProps.timeObj.statResultObj.setString("N.A.");
		m_overlayProps.timeObj.statResultObj.setColor(sf::Color::Red);
	}
	else if(m_hasRenderStarted && m_hasPixelSampleCollectionCompleted && !m_hasRenderCompleted)
	{
		m_overlayProps.renderObj.statResultObj.setString("II - Color computation.");
		m_overlayProps.renderObj.statResultObj.setColor(sf::Color::Magenta);
		m_overlayProps.timeObj.statResultObj.setString("N.A.");
		m_overlayProps.timeObj.statResultObj.setColor(sf::Color::Magenta);
	}

	else if (m_hasRenderCompleted)
	{
		std::string timerResult{ timerObj.getTimerResultString() };

		m_overlayProps.timeObj.statResultObj.setString(timerResult);
		m_overlayProps.timeObj.statResultObj.setColor(sf::Color::Green);

		m_overlayProps.renderObj.statResultObj.setString("Render complete.");
		m_overlayProps.renderObj.statResultObj.setColor(sf::Color::Green);
	}

	const double scaleSpacingFactor{ 0.22 * pixResObj.widthInPixels };
	const double overlayTitleStartPos{ pixResObj.widthInPixels - scaleSpacingFactor };
	const double overlayResultStartPos{ overlayTitleStartPos + (0.12 * overlayTitleStartPos) };

	for (auto& overlayStat : tempCopy)
	{
		overlayStat.get().statTitleObj.setPosition(overlayTitleStartPos, verticalStatSpacing);
		overlayStat.get().statResultObj.setPosition(overlayResultStartPos, verticalStatSpacing);

		renderWindowObj.draw(overlayStat.get().statTitleObj);
		renderWindowObj.draw(overlayStat.get().statResultObj);

		verticalStatSpacing += 30;
	}
}

void StatsOverlay::setRenderingCompleteStatus(bool hasRenderCompleted) noexcept
{
	m_hasRenderCompleted = hasRenderCompleted;
}

void StatsOverlay::setRenderingStartStatus(bool hasRenderStarted) noexcept
{
	m_hasRenderStarted = hasRenderStarted;
}

void StatsOverlay::setPixelSampleCollectionCompleteStatus(bool hasPixelSampleCollectionCompleted) noexcept
{
	m_hasPixelSampleCollectionCompleted = hasPixelSampleCollectionCompleted;
}

void StatsOverlay::setOverlayVisibility(bool shouldDisplayOverlay) noexcept
{
	m_isOverlayEnabled = shouldDisplayOverlay;
}

bool StatsOverlay::getOverlayVisibility() const noexcept
{
	return m_isOverlayEnabled;
}

void StatsOverlay::setTotalCPUUsage(double totalCPUUsage) noexcept
{
	m_totalCPUUsage = totalCPUUsage;
}