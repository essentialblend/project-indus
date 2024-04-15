export module renderer;

import <iostream>;
import <vector>;
import <print>;

import core_constructs;
import color;
import ray;
import camera;
import vec3;
import window;
import overlay;
import timer;

import <SFML/Graphics.hpp>;

export class Renderer
{
public:
	explicit Renderer() noexcept = default;
    explicit Renderer(const Camera& cameraObj) noexcept : m_rendererCam{ cameraObj } {}

    void renderSFMLWindow();

    void renderFrame(const PixelResolution& pixResObj, const PixelDimension& pixDimObj, const Point& camCenter, std::vector<Color>& localPixelBuffer, const Timer& timerObj);

private:
    Camera m_rendererCam{};
    SFMLWindow m_sfmlWindow{};
    Overlay m_statsOverlay{};   

    [[nodiscard]] Color computeRayColor(const Ray& inputRay) const;
};

Color Renderer::computeRayColor(const Ray& inputRay) const
{
    const Color gradientColor{ 0.5, 0.7, 1.0 };

    const Vec3 inputRayDir{ getUnit(inputRay.getDirection()) };
    const double lerpFactor{ 0.5 * (inputRayDir.getY() + 1.0) };
    const Vec3 returnedColor{ ((1.0 - lerpFactor) * Color(1.0).getBaseVec()) + (lerpFactor * gradientColor.getBaseVec()) };

    const Color finalReturnColor(returnedColor.getX(), returnedColor.getY(), returnedColor.getZ());

    return finalReturnColor;
}

void Renderer::renderSFMLWindow()
{
    Timer renderTimer;

    auto& renderWindowObj = m_sfmlWindow.getWindowProperties().renderWindowObj;
    const auto& pixResObj = m_rendererCam.getCameraProperties().camImgPropsObj.pixelResolutionObj;
    const auto& pixDimObj = m_rendererCam.getCameraProperties().camPixelDimObj;
    const auto& camCenter = m_rendererCam.getCameraProperties().camCenter;

    std::vector<Color> localPixelBuffer;
    localPixelBuffer.reserve(static_cast<long long>(pixResObj.widthInPixels * pixResObj.heightInPixels));

    bool hasRenderCompleted{ false };

    m_sfmlWindow = SFMLWindow(m_rendererCam.getCameraProperties(), "indus prelim");
    m_sfmlWindow.setupWindow();

    m_statsOverlay.setupOverlay();

    renderTimer.startTimer();

    while (renderWindowObj->isOpen())
    {
        sf::Event event{};
        while (renderWindowObj->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                renderWindowObj->close();
        }

        while (!hasRenderCompleted)
        {
            renderFrame(pixResObj, pixDimObj, camCenter, localPixelBuffer, renderTimer);
            
            renderTimer.endTimer();

            hasRenderCompleted = true;
            m_statsOverlay.setRenderingStatus(!hasRenderCompleted);
        }

        //m_sfmlWindow.displayWindow(m_statsOverlay, renderTimer);
    }

        renderTimer.resetTimer();
}

void Renderer::renderFrame(const PixelResolution& pixResObj, const PixelDimension& pixDimObj, const Point& camCenter, std::vector<Color>& localPixelBuffer, const Timer& timerObj)
{
    for (int i = 0; i < pixResObj.heightInPixels; ++i) {
        for (int j = 0; j < pixResObj.widthInPixels; ++j) {

            const Point currentPixelCenter{ pixDimObj.pixelCenter + (j * pixDimObj.lateralSpanInAbsVal) + (i * pixDimObj.verticalSpanInAbsVal) };
            const Ray currentPixelRay{ camCenter, currentPixelCenter - camCenter };

            const Color normCurrentPixelColor{ computeRayColor(currentPixelRay) };

            sf::Uint8 pixelData[4] =
            {
                static_cast<sf::Uint8>(normCurrentPixelColor.getBaseVec().getX() * 255),
                static_cast<sf::Uint8>(normCurrentPixelColor.getBaseVec().getY() * 255),
                static_cast<sf::Uint8>(normCurrentPixelColor.getBaseVec().getZ() * 255),
                255
            };

            localPixelBuffer.push_back(std::move(normCurrentPixelColor));

            m_sfmlWindow.getWindowProperties().texObj->update(pixelData, 1, 1, j, i);
        }

        m_sfmlWindow.displayWindow(m_statsOverlay, timerObj);
    }
}


