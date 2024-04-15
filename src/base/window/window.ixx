export module window;

import <string_view>;

import core_constructs;
import vec3;
import ray;
import overlay;
import timer;

import <SFML/Graphics.hpp>;

export class SFMLWindow
{
public:
	explicit SFMLWindow() noexcept = default;
	explicit SFMLWindow(const CameraProperties& camPropsObj, std::string_view windowTitleString) noexcept : m_cameraPropsObj{ camPropsObj }, m_windowTitle{ windowTitleString } {}

	void setupWindow();
    void displayWindow(Overlay& statsOverlayObj, const Timer& timerObj);

    SFMLWindowProperties& getWindowProperties() noexcept;

private:
    SFMLWindowProperties m_windowProps{};
	CameraProperties m_cameraPropsObj{};
    std::string m_windowTitle{};
};

void SFMLWindow::setupWindow()
{
    // Local copies for readability.
    const auto& pixelResObj = m_cameraPropsObj.camImgPropsObj.pixelResolutionObj;

    m_windowProps.renderWindowObj = std::make_unique<sf::RenderWindow>(sf::VideoMode(static_cast<int>(pixelResObj.widthInPixels * m_windowProps.windowedResScale), static_cast<int>(pixelResObj.heightInPixels * m_windowProps.windowedResScale)), m_windowTitle);
    m_windowProps.viewObj = std::make_unique<sf::View>(sf::FloatRect(0, 0, static_cast<float>(pixelResObj.widthInPixels), static_cast<float>(pixelResObj.heightInPixels)));
    m_windowProps.texObj = std::make_unique<sf::Texture>();
    m_windowProps.spriteObj = std::make_unique<sf::Sprite>();
    
    m_windowProps.renderWindowObj->setView(*m_windowProps.viewObj);
    m_windowProps.renderWindowObj->setFramerateLimit(m_windowProps.prefFPSInIntegral);
    m_windowProps.texObj->create(pixelResObj.widthInPixels, pixelResObj.heightInPixels);
    m_windowProps.spriteObj->setTexture(*m_windowProps.texObj);
}

void SFMLWindow::displayWindow(Overlay& statsOverlayObj, const Timer& timerObj)
{
    m_windowProps.renderWindowObj->clear();
    m_windowProps.renderWindowObj->draw(*m_windowProps.spriteObj);
    //statsOverlayObj.showOverlay(*m_windowProps.renderWindowObj, m_cameraPropsObj.camImgPropsObj.pixelResolutionObj, timerObj);
    m_windowProps.renderWindowObj->display();
}

SFMLWindowProperties& SFMLWindow::getWindowProperties() noexcept
{
    return m_windowProps;
}
