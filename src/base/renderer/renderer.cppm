import renderer;

import <span>;
import <iostream>;
import <thread>;
import <future>;
import <memory>;

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

void Renderer::renderFrameMultiCore(const PixelResolution& pixResObj, const PixelDimension& pixDimObj, const Point& camCenter, std::vector<Color>& primaryPixelBuffer)
{
    std::vector<sf::Uint8> tempTexUpdateBuffer;
    const unsigned int numAvailableThreads = std::thread::hardware_concurrency();
    const unsigned int numThreadsToUse = numAvailableThreads > 1 ? static_cast<int>(numAvailableThreads * 0.5) : 1;

    MT_ThreadPool renderThreadPool(numThreadsToUse);

    for (int eachPixelRow{}; eachPixelRow < pixResObj.heightInPixels; ++eachPixelRow)
    {
        renderThreadPool.enqueueThreadPoolTask([this, &pixResObj, &pixDimObj, &camCenter, &primaryPixelBuffer, eachPixelRow]() {
            this->renderPixelRowThreadPoolTask(pixResObj, pixDimObj, camCenter, primaryPixelBuffer, eachPixelRow);
            });
    }
    renderThreadPool.stopThreadPool();
}

void Renderer::setRendererFunctors(const RendererSFMLFunctors& rendererFuncObj) noexcept
{
    m_rendererFunctors = rendererFuncObj;
}

void Renderer::renderPixelRowThreadPoolTask(const PixelResolution& pixResObj, const PixelDimension& pixDimObj, const Point& camCenter, std::vector<Color>& primaryPixelBuffer, int columnPixelForNewRow) const
{
    for (int i{}; i < pixResObj.widthInPixels; ++i)
    {
        const Point currentPixelCenter{ pixDimObj.pixelCenter + (i * pixDimObj.lateralSpanInAbsVal) + (columnPixelForNewRow * pixDimObj.verticalSpanInAbsVal) };
        const Ray currentPixelRay{ camCenter, currentPixelCenter - camCenter };

        const Color normCurrPixColor{ computeRayColor(currentPixelRay) };

        const int primaryBufferIndex{ ((columnPixelForNewRow * pixResObj.widthInPixels) + i) };

        primaryPixelBuffer[primaryBufferIndex] = normCurrPixColor;
    }
}