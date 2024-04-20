import stats_overlay;

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
	m_overlayProps.titleObj.statTitleObj.setString("indus:");
	m_overlayProps.titleObj.statResultObj.setString("v0.0.0-alpha");
	m_overlayProps.modeObj.statTitleObj.setString("Mode: ");

	if(isMultithreaded)
	{
		m_overlayProps.modeObj.statResultObj.setString("Multi-threaded.");
	}
	else
	{
		m_overlayProps.modeObj.statResultObj.setString("Single-threaded.");
	}
	
	m_overlayProps.timeObj.statTitleObj.setString("Time: ");
	m_overlayProps.timeObj.statResultObj.setString("Awaiting render.");
	m_overlayProps.timeObj.statResultObj.setColor(sf::Color::Green);
	m_overlayProps.renderObj.statTitleObj.setString("Status: ");
	m_overlayProps.renderObj.statResultObj.setString("Idle.");
	m_overlayProps.renderObj.statResultObj.setColor(sf::Color::Green);
	m_overlayProps.GUIObj.statTitleObj.setString("GUI Status: ");
	m_overlayProps.GUIObj.statResultObj.setString("Responsive.");
	m_overlayProps.GUIObj.statResultObj.setColor(sf::Color::Green);
}

void StatsOverlay::setSFMLTextProperties()
{
	std::vector<std::reference_wrapper<OverlayStatistic>> localStatsCollection{ m_overlayProps.getStatsCollection() };

	for (auto& overlayStat : localStatsCollection)
	{
		overlayStat.get().statTitleObj.setFont(m_overlayFont);
		overlayStat.get().statTitleObj.setCharacterSize(25);
		overlayStat.get().statTitleObj.setColor(sf::Color::White);

		overlayStat.get().statResultObj.setFont(m_overlayFont);
		overlayStat.get().statResultObj.setCharacterSize(25);
		overlayStat.get().statResultObj.setColor(sf::Color::White);
	}
}

void StatsOverlay::showOverlay(sf::RenderWindow& renderWindowObj, const PixelResolution& pixResObj, const Timer& timerObj)
{
	if (!m_isOverlayEnabled)
	{
		return;
	}

	if (!m_hasRenderCompleted)
	{
		std::vector<std::reference_wrapper<OverlayStatistic>> tempCopy{ m_overlayProps.getStatsCollection() };

		float widthScaleFactor{ 0.23f };
		float overlayWidthPadding{ pixResObj.widthInPixels * widthScaleFactor };
		float modeTitleGlobalWidth = m_overlayProps.modeObj.statTitleObj.getGlobalBounds().width + 100;
		float resultTextStartPos{ (pixResObj.widthInPixels - overlayWidthPadding) + modeTitleGlobalWidth };
		int verticalStatSpacing{ 0 };

		for (auto& overlayStat : tempCopy)
		{
			overlayStat.get().statTitleObj.setPosition(static_cast<float>(pixResObj.widthInPixels - overlayWidthPadding), static_cast<float>(verticalStatSpacing));
			overlayStat.get().statResultObj.setPosition(static_cast<float>(resultTextStartPos), static_cast<float>(verticalStatSpacing));

			renderWindowObj.draw(overlayStat.get().statTitleObj);
			renderWindowObj.draw(overlayStat.get().statResultObj);

			verticalStatSpacing += 30;
		}
	}
	else
	{
		std::vector<std::reference_wrapper<OverlayStatistic>> tempCopy{ m_overlayProps.getStatsCollection() };

		float widthScaleFactor{ 0.23f };
		float overlayWidthPadding{ pixResObj.widthInPixels * widthScaleFactor };
		float modeTitleGlobalWidth = m_overlayProps.modeObj.statTitleObj.getGlobalBounds().width + 100;
		float resultTextStartPos{ (pixResObj.widthInPixels - overlayWidthPadding) + modeTitleGlobalWidth };
		int verticalStatSpacing{ 0 };

		std::string timerResult{ timerObj.getTimerResultString() };

		m_overlayProps.timeObj.statResultObj.setString(timerResult);
		m_overlayProps.timeObj.statResultObj.setColor(sf::Color::Green);

		m_overlayProps.renderObj.statResultObj.setString("Render complete.");
		m_overlayProps.renderObj.statResultObj.setColor(sf::Color::Green);

		m_overlayProps.GUIObj.statResultObj.setString("Responsive.");
		m_overlayProps.GUIObj.statResultObj.setColor(sf::Color::Green);

		for (auto& overlayStat : tempCopy)
		{
			overlayStat.get().statTitleObj.setPosition(static_cast<float>(pixResObj.widthInPixels - overlayWidthPadding), static_cast<float>(verticalStatSpacing));
			overlayStat.get().statResultObj.setPosition(static_cast<float>(resultTextStartPos), static_cast<float>(verticalStatSpacing));

			renderWindowObj.draw(overlayStat.get().statTitleObj);
			renderWindowObj.draw(overlayStat.get().statResultObj);

			verticalStatSpacing += 30;
		}
	}
}

void StatsOverlay::setRenderingStatus(bool hasRenderCompleted) noexcept
{
	m_hasRenderCompleted = hasRenderCompleted;
}

void StatsOverlay::setOverlayVisibility(bool shouldDisplayOverlay) noexcept
{
	m_isOverlayEnabled = shouldDisplayOverlay;
}

bool StatsOverlay::getOverlayVisibility() const noexcept
{
	return m_isOverlayEnabled;
}