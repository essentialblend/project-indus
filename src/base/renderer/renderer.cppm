import renderer;

import <span>;
import <iostream>;
import <thread>;
import <future>;
import <memory>;
import <algorithm>;
import <execution>;
import <print>;

import core_constructs;
import color;
import threadpool;

import <SFML/Graphics.hpp>;

Color Renderer::computeRayColor(const Ray& inputRay)
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

    m_texUpdateFutureVec.reserve(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);
    m_texUpdateLatch = std::make_unique<std::latch>(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);

}

void Renderer::renderFrameSingleCore(const PixelResolution& pixResObj, const PixelDimension& pixDimObj, const Point& camCenter, std::vector<Color>& primaryPixelBuffer)
{
    int numRowsPerTexUpdateBatch{ 20 };
    int currentNumRowsParsed{ 0 };

    std::vector<sf::Uint8> tempTexUpdateBuffer;
    tempTexUpdateBuffer.reserve(static_cast<long long>(pixResObj.widthInPixels * numRowsPerTexUpdateBatch * (m_mainCamera.getCameraProperties().camImgPropsObj.numColorChannels + 1)));

    for (int i = 0; i < pixResObj.heightInPixels; ++i) {
        for (int j = 0; j < pixResObj.widthInPixels; ++j) {

            const Point currentPixelCenter{ pixDimObj.pixelCenter + (j * pixDimObj.lateralSpanInAbsVal) + (i * pixDimObj.verticalSpanInAbsVal) };
            const Ray currentPixelRay{ camCenter, currentPixelCenter - camCenter };

            // Prime candidate for parallelization.
            const Color normCurrPixColor{ computeRayColor(currentPixelRay) };

            tempTexUpdateBuffer.push_back(static_cast<sf::Uint8>(normCurrPixColor.getBaseVec().getX() * 255));
            tempTexUpdateBuffer.push_back(static_cast<sf::Uint8>(normCurrPixColor.getBaseVec().getY() * 255));
            tempTexUpdateBuffer.push_back(static_cast<sf::Uint8>(normCurrPixColor.getBaseVec().getZ() * 255));
            tempTexUpdateBuffer.push_back(255);

            primaryPixelBuffer.push_back(std::move(normCurrPixColor));
        }
        ++currentNumRowsParsed;

        if (currentNumRowsParsed == numRowsPerTexUpdateBatch)
        {
            unsigned int startYCoord = i - numRowsPerTexUpdateBatch + 1;
            m_rendererFunctors.sfmlTextureUpdateFunctor(tempTexUpdateBuffer.data(), pixResObj.widthInPixels, currentNumRowsParsed, 0, startYCoord);
            tempTexUpdateBuffer.clear();
            currentNumRowsParsed = 0;

            m_rendererFunctors.sfmlClearWindowFunctor();
            m_rendererFunctors.sfmlDrawSpriteFunctor();
            m_rendererFunctors.sfmlDisplayWindowFunctor();
        }
    }

    if (!tempTexUpdateBuffer.empty())
    {
        unsigned int startYCoord = pixResObj.heightInPixels - currentNumRowsParsed;

        m_rendererFunctors.sfmlTextureUpdateFunctor(tempTexUpdateBuffer.data(), pixResObj.widthInPixels, currentNumRowsParsed, 0, startYCoord);

        tempTexUpdateBuffer.clear();
        currentNumRowsParsed = 0;
    }
}

void Renderer::renderFrameMultiCore(std::vector<Color>& mainFramebuffer)
{
    m_renderThreadPool.initiateThreadPool();

    for (int eachPixelRow{}; eachPixelRow < m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels; ++eachPixelRow)
    {
        m_texUpdateFutureVec.push_back(m_renderThreadPool.enqueueThreadPoolTask([this, eachPixelRow, &mainFramebuffer]() {
            this->renderPixelRowThreadPoolTask(eachPixelRow, mainFramebuffer);
        }));    
    }
 }

void Renderer::setRendererSFMLFunctors(const RendererSFMLFunctors& rendererFuncObj) noexcept
{
    m_rendererFunctors = rendererFuncObj;
}

void Renderer::renderPixelRowThreadPoolTask(int currentRowCount, std::vector<Color>& mainFramebuffer)
{
    const auto localCamProps{ m_mainCamera.getCameraProperties() };
    const auto localPixResObj{ m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj };
    const auto localPixDimObj{ m_mainCamera.getCameraProperties().camPixelDimObj };
    m_texUpdateLatch->count_down();
    for (int i{}; i < localPixResObj.widthInPixels; ++i)
    {
        const Point currentPixelCenter{ m_mainCamera.getCameraProperties().camPixelDimObj.pixelCenter + (i * localPixDimObj.lateralSpanInAbsVal) + (currentRowCount * localPixDimObj.verticalSpanInAbsVal)};
        const Ray currentPixelRay{ localCamProps.camCenter, currentPixelCenter - localCamProps.camCenter };
        const Color normCurrPixColor{ computeRayColor(currentPixelRay) };
        const int index { ((currentRowCount * localPixResObj.widthInPixels) + i) };
        mainFramebuffer[index] = normCurrPixColor;
    }
    
}

void Renderer::setThreadingMode(bool isMultithreaded) noexcept
{
    m_isMultithreaded = isMultithreaded;
}

bool Renderer::getThreadingMode() const noexcept
{
    return m_isMultithreaded;
}

bool Renderer::checkForDrawUpdate()
{
    auto shouldStart = m_texUpdateLatch->try_wait();
    if (!shouldStart) return false;

    const auto itBegin{ m_texUpdateFutureVec.begin() + m_currChunkForTexUpdate };
    auto itEnd{ itBegin };
    const auto remainingDistance = std::distance(itBegin, m_texUpdateFutureVec.end());
    remainingDistance < m_texUpdateRate ? 
        itEnd = m_texUpdateFutureVec.end() : 
        itEnd = itBegin + m_texUpdateRate;

    const bool hasChunkCompleted = std::all_of(itBegin, itEnd, [](const std::future<void>& fut) {
    return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    });

    if (!hasChunkCompleted) return false;

    itEnd == m_texUpdateFutureVec.end() ? m_currChunkForTexUpdate = 0 : m_currChunkForTexUpdate += m_texUpdateRate;

    if (itEnd == m_texUpdateFutureVec.end()) m_renderThreadPool.stopThreadPool();

    return true;
}

std::pair<int, int> Renderer::getTextureUpdateCounters() const
{
    return std::pair<int, int>(m_currChunkForTexUpdate - m_texUpdateRate, m_texUpdateRate);
}