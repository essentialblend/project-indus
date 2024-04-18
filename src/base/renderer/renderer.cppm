import renderer;

import <span>;
import <iostream>;

import core_constructs;
import color;

import <SFML/Graphics.hpp>;

Color Renderer::computeRayColor(const Ray& inputRay) const
{
    const Color gradientColor{ 0.5, 0.7, 1.0 };

    const Vec3 inputRayDir{ getUnit(inputRay.getDirection()) };
    const double lerpFactor{ 0.5 * (inputRayDir.getY() + 1.0) };
    const Vec3 returnedColor{ ((1.0 - lerpFactor) * Color(1.0).getBaseVec()) + (lerpFactor * gradientColor.getBaseVec()) };

    const Color finalReturnColor(returnedColor.getX(), returnedColor.getY(), returnedColor.getZ());

    return finalReturnColor;
}

CameraProperties Renderer::getRendererCameraProps() const noexcept
{
    return m_mainCamera.getCameraProperties();
}

void Renderer::setupRenderer(const PixelResolution& pixResObj, const AspectRatio& aspectRatioObj)
{
    m_mainCamera = Camera{ pixResObj, aspectRatioObj };
    m_mainCamera.setupCamera();
}
//void Renderer::renderSFMLWindow()
//{
//    Timer renderTimer;
//
//    auto& renderWindowObj = m_sfmlWindow.getWindowProperties().renderWindowObj;
//    const auto& pixResObj = m_rendererCam.getCameraProperties().camImgPropsObj.pixelResolutionObj;
//    const auto& pixDimObj = m_rendererCam.getCameraProperties().camPixelDimObj;
//    const auto& camCenter = m_rendererCam.getCameraProperties().camCenter;
//
//    std::vector<Color> localPixelBuffer;
//    localPixelBuffer.reserve(static_cast<long long>(pixResObj.widthInPixels * pixResObj.heightInPixels));
//
//    bool hasRenderCompleted{ false };
//
//    m_sfmlWindow.setupWindow();
//
//    m_statsOverlay.setupOverlay();
//
//    renderTimer.startTimer();
//
//    while (renderWindowObj.isOpen())
//    {
//        sf::Event event{};
//        while (renderWindowObj.pollEvent(event))
//        {
//            if (event.type == sf::Event::Closed)
//                renderWindowObj.close();
//
//            if (event.type == sf::Event::KeyPressed)
//            {
//                if (event.key.code == sf::Keyboard::BackSpace)
//                {
//                    m_statsOverlay.setOverlayVisibility(!m_statsOverlay.getOverlayVisibility());
//                }
//            }
//        }
//
//        while (!hasRenderCompleted)
//        {
//            renderFrame(pixResObj, pixDimObj, camCenter, localPixelBuffer, renderTimer);
//
//            renderTimer.endTimer();
//
//            hasRenderCompleted = true;
//            m_statsOverlay.setRenderingStatus(hasRenderCompleted);
//        }
//
//        m_sfmlWindow.displayWindow(m_statsOverlay, renderTimer);
//    }
//    renderTimer.resetTimer();
//}

void Renderer::renderFrame(const PixelResolution& pixResObj, const PixelDimension& pixDimObj, const Point& camCenter, std::vector<Color>& primaryPixelBuffer)
{
    int numRowsPerTexUpdateBatch{ 5 };
    int currentNumRowsParsed{ 0 };

    std::vector<sf::Uint8> tempTexUpdateBuffer;
    tempTexUpdateBuffer.reserve(static_cast<long long>(pixResObj.widthInPixels * numRowsPerTexUpdateBatch * (m_mainCamera.getCameraProperties().camImgPropsObj.numColorChannels + 1)));

    for (int i = 0; i < pixResObj.heightInPixels; ++i) {
        for (int j = 0; j < pixResObj.widthInPixels; ++j) {

            const Point currentPixelCenter{ pixDimObj.pixelCenter + (j * pixDimObj.lateralSpanInAbsVal) + (i * pixDimObj.verticalSpanInAbsVal) };
            const Ray currentPixelRay{ camCenter, currentPixelCenter - camCenter };

            const Color normCurrentPixelColor{ computeRayColor(currentPixelRay) };

            tempTexUpdateBuffer.push_back(static_cast<sf::Uint8>(normCurrentPixelColor.getBaseVec().getX() * 255));
            tempTexUpdateBuffer.push_back(static_cast<sf::Uint8>(normCurrentPixelColor.getBaseVec().getY() * 255));
            tempTexUpdateBuffer.push_back(static_cast<sf::Uint8>(normCurrentPixelColor.getBaseVec().getZ() * 255));
            tempTexUpdateBuffer.push_back(255);

            primaryPixelBuffer.push_back(std::move(normCurrentPixelColor));
        }
        ++currentNumRowsParsed;

        if (currentNumRowsParsed == numRowsPerTexUpdateBatch)
        {
            unsigned int startYCoord = i - numRowsPerTexUpdateBatch + 1;
            m_textureUpdateFunctor(tempTexUpdateBuffer.data(), pixResObj.widthInPixels, currentNumRowsParsed, 0, startYCoord);
            tempTexUpdateBuffer.clear();
            currentNumRowsParsed = 0;
        }
    }

    if (!tempTexUpdateBuffer.empty())
    {
        unsigned int startYCoord = pixResObj.heightInPixels - currentNumRowsParsed;

        m_textureUpdateFunctor(tempTexUpdateBuffer.data(), pixResObj.widthInPixels, currentNumRowsParsed, 0, startYCoord);

        tempTexUpdateBuffer.clear();
        currentNumRowsParsed = 0;
    }
}

void Renderer::setTextureUpdateFunctor(const std::function<void(const sf::Uint8*, unsigned int, unsigned int, unsigned int, unsigned int)>& textureUpdateFunctor) noexcept
{
    m_textureUpdateFunctor = textureUpdateFunctor;
};