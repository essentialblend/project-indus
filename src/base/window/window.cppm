import window;

import <SFML/Graphics.hpp>;

void SFMLWindow::setupWindow()
{
	m_windowProps.viewObj = sf::View(sf::FloatRect(0, 0, static_cast<float>(m_windowPixelRes.widthInPixels), static_cast<float>(m_windowPixelRes.heightInPixels)));

	m_windowProps.texObj.create(m_windowPixelRes.widthInPixels, m_windowPixelRes.heightInPixels);
	m_windowProps.spriteObj = sf::Sprite();


}

void SFMLWindow::displayWindow(StatsOverlay& statsOverlayObj, const Timer& timerObj)
{
	m_windowProps.renderWindowObj.create(sf::VideoMode(static_cast<int>(m_windowPixelRes.widthInPixels * m_windowProps.windowedResScale), static_cast<int>(m_windowPixelRes.heightInPixels * m_windowProps.windowedResScale)), m_windowTitle);

	m_windowProps.renderWindowObj.setView(m_windowProps.viewObj);
	m_windowProps.renderWindowObj.setFramerateLimit(m_windowProps.prefFPSInIntegral);
	m_windowProps.texObj.create(m_windowPixelRes.widthInPixels, m_windowPixelRes.heightInPixels);
	m_windowProps.spriteObj.setTexture(m_windowProps.texObj);

	while (m_windowProps.renderWindowObj.isOpen())
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

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					m_renderFrameFunctor();
				}
			}

		}

		m_windowProps.renderWindowObj.clear();
		m_windowProps.renderWindowObj.draw(m_windowProps.spriteObj);
		statsOverlayObj.showOverlay(m_windowProps.renderWindowObj, m_windowPixelRes, timerObj);
		m_windowProps.renderWindowObj.display();
	}

}

SFMLWindowProperties& SFMLWindow::getSFMLWindowProperties() noexcept
{
	return m_windowProps;
}