module eu_statsoverlay;

import <string>;
import <iostream>;
import <iomanip>;
import <sstream>;
import <any>;

void EUStatsOverlay::initializeEntity([[maybe_unused]] const std::vector<std::any>& args)
{
	for(const auto& [title, result] : m_defaultStatsContentVector)
	{
		addStatistic(title, result);
	}

	setSFMLTextObjectProperties();
}

void EUStatsOverlay::flipDisplayStatus() noexcept
{
	m_displayStatus = !m_displayStatus;
}

void EUStatsOverlay::showOverlay(const std::vector<std::any>& args)
{
	auto& renderWindowObj{ std::any_cast<std::reference_wrapper<sf::RenderWindow>>(args[0]).get() };
	std::size_t maxTitleWidth{};

	std::for_each(m_overlayStats.begin(), m_overlayStats.end(), [&maxTitleWidth](const auto& stat)
	{
		if (stat.getTitle().size() > maxTitleWidth)
		{
			maxTitleWidth = (stat.getTitle().size());
		}
	});

	const auto& windowSize{ renderWindowObj.getView().getSize() };
	const auto& widthScaleFac{ windowSize.x * 0.22f };
	const auto& heightScaleFac{ windowSize.y * 0.02f };
	const auto& fieldVerticalSeparatorFac{ windowSize.y * 0.03f };

	const float xPosition{ static_cast<float>(windowSize.x - widthScaleFac) };
	float yPosition{ heightScaleFac };

	auto pdhFormattedStrPair{ getFormattedStrForPDHVars() };

	// Draw overlay background fill-rect.
	sf::RectangleShape backgroundRect;
	backgroundRect.setSize(sf::Vector2f(420, 205));
	backgroundRect.setFillColor(sf::Color(255, 0, 255, 50));
	backgroundRect.setPosition(xPosition - 10.0f, 15);

	renderWindowObj.draw(backgroundRect);

	// Draw overlay.
	for(auto& stat : m_overlayStats)
	{
		const int remainingWidth{ static_cast<int>(maxTitleWidth - stat.getTitle().size()) };
		const std::string formattedTitle{ stat.getTitle() + std::string(remainingWidth + 1, ' ') + ":" };
		m_overlayTitleResultPair.first.setString(formattedTitle);
		m_overlayTitleResultPair.first.setPosition(xPosition, yPosition);

		if (stat.getTitle() == "Total CPU")
		{
			m_overlayTitleResultPair.second.setString(std::string(1, ' ') + pdhFormattedStrPair.first);
		}
		else if(stat.getTitle() == "Total DRAM")
		{
			m_overlayTitleResultPair.second.setString(std::string(1, ' ') + pdhFormattedStrPair.second);
		}
		else m_overlayTitleResultPair.second.setString(std::string(1, ' ') + stat.getResult());

		m_overlayTitleResultPair.second.setPosition(xPosition + widthScaleFac * 0.4f, yPosition);

		renderWindowObj.draw(m_overlayTitleResultPair.first);
		renderWindowObj.draw(m_overlayTitleResultPair.second);

		yPosition += fieldVerticalSeparatorFac;
	}
}

void EUStatsOverlay::updateOverlayContent([[maybe_unused]] const std::vector<std::any>& args)
{
	m_totalCPUUsage = std::any_cast<double>(args[0]);
	m_totalDRAMGB = std::any_cast<double>(args[1]);
	m_usedDRAMGB = std::any_cast<double>(args[2]);
}

std::pair<sf::Text, sf::Text>& EUStatsOverlay::getOverlayTitleResultPairMutable() noexcept
{
	return m_overlayTitleResultPair;
}

bool EUStatsOverlay::getOverlayVisibility() const noexcept
{
	return m_displayStatus;
}

void EUStatsOverlay::setSFMLTextObjectProperties()
{
	if (!m_overlayFont.loadFromFile("src/dep/CourierPrime-Regular.ttf"))
	{
		throw std::runtime_error("Failed to load font file.");
	}

	m_overlayTitleResultPair.first.setFont(m_overlayFont);
	m_overlayTitleResultPair.first.setCharacterSize(22);
	m_overlayTitleResultPair.first.setColor(sf::Color::White);

	m_overlayTitleResultPair.second.setFont(m_overlayFont);
	m_overlayTitleResultPair.second.setCharacterSize(22);
	m_overlayTitleResultPair.second.setColor(sf::Color::White);
}

void EUStatsOverlay::addStatistic(const std::string_view titleStr, const std::string_view resultStr)
{
	m_overlayStats.emplace_back(titleStr.data(), resultStr.data());
}

std::pair<std::string, std::string> EUStatsOverlay::getFormattedStrForPDHVars()
{
	std::ostringstream osStreamCPU{};
	osStreamCPU << std::fixed << std::setprecision(2) << m_totalCPUUsage;
	std::ostringstream osStreamUsedDRAM;
	osStreamUsedDRAM << std::fixed << std::setprecision(2) << m_usedDRAMGB;
	std::ostringstream osStreamTotalDRAM;
	osStreamTotalDRAM << std::fixed << std::setprecision(2) << m_totalDRAMGB;

	return std::pair<std::string, std::string>{osStreamCPU.str() + "%.", osStreamUsedDRAM.str() + " / " + osStreamTotalDRAM.str() + " GB."};
}

