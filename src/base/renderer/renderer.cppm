import renderer;

import core_util;
import color;
import threadpool;
import hit_record;
import material;
import lambertian;
import world_object;

import <SFML/Graphics.hpp>;

Color Renderer::computeRayColor(const Ray& inputRay, const WorldObject& mainWorld, int maxRayBounceDepth)
{
    if (maxRayBounceDepth <= 0) return Color(0);

    HitRecord tempHitRecord{};
    
    if (mainWorld.checkHit(inputRay, Interval(0.001, +UInfinity), tempHitRecord))
    {
        Ray scatteredRay{};
        Color materialAlbedo{};

        if (tempHitRecord.hitMaterial->handleRayScatter(inputRay, tempHitRecord, materialAlbedo, scatteredRay))
        {
            return materialAlbedo * computeRayColor(scatteredRay, mainWorld, maxRayBounceDepth - 1);
        }
    }
    return getBackgroundGradient(inputRay);
}

const Color Renderer::getBackgroundGradient(const Ray& inputRay)
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

    m_renderingStatusFutureVec.reserve(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);
    m_texUpdateLatch = std::make_unique<std::latch>(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);

}

void Renderer::renderFrameMultiCore(std::vector<Color>& mainFramebuffer, const WorldObject& mainWorld)
{
    m_renderThreadPool.initiateThreadPool();

    for (int eachPixelRow{}; eachPixelRow < m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels; ++eachPixelRow)
    {
        m_renderingStatusFutureVec.push_back(m_renderThreadPool.enqueueThreadPoolTask([this, eachPixelRow, &mainFramebuffer, &mainWorld]() {
            this->renderPixelRowThreadPoolTask(eachPixelRow, mainFramebuffer, mainWorld);
        }));    
    }
 }

void Renderer::setRendererSFMLFunctors(const RendererSFMLFunctors& rendererFuncObj) noexcept
{
    m_rendererFunctors = rendererFuncObj;
}

void Renderer::renderPixelRowThreadPoolTask(int currentRowCount, std::vector<Color>& mainFramebuffer, const WorldObject& mainWorld)
{
    const auto localCamProps{ m_mainCamera.getCameraProperties() };
    const auto localPixResObj{ m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj };
    const auto localPixDimObj{ m_mainCamera.getCameraProperties().camPixelDimObj };
    m_texUpdateLatch->count_down();

    for (int i{}; i < localPixResObj.widthInPixels; ++i)
    {
        Color normCurrPixColor(0);
        int index{};
        for (int pixelSample{}; pixelSample < m_samplesPerPixel; ++pixelSample)
        {
            Ray currentPixelRay = getRayForPixel(i, currentRowCount);
            normCurrPixColor = normCurrPixColor + computeRayColor(currentPixelRay, mainWorld, m_maxRayBounceDepth - 1);
            index = ((currentRowCount * localPixResObj.widthInPixels) + i);
        }
        mainFramebuffer[index] = normCurrPixColor * (1.0 / m_samplesPerPixel);
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

    const auto itBegin{ m_renderingStatusFutureVec.begin() + m_currChunkForTexUpdate };
    auto itEnd{ itBegin };
    const auto remainingDistance = std::distance(itBegin, m_renderingStatusFutureVec.end());
    remainingDistance < m_texUpdateRate ? 
        itEnd = m_renderingStatusFutureVec.end() : 
        itEnd = itBegin + m_texUpdateRate;

    const bool hasChunkCompleted = std::all_of(itBegin, itEnd, [](const std::future<void>& fut) {
    return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    });

    if (!hasChunkCompleted) return false;

    itEnd == m_renderingStatusFutureVec.end() ? m_currChunkForTexUpdate = 0 : m_currChunkForTexUpdate += m_texUpdateRate;

    if (itEnd == m_renderingStatusFutureVec.end()) m_renderThreadPool.stopThreadPool();

    return hasChunkCompleted;
}

std::pair<int, int> Renderer::getTextureUpdateCounters() const
{
    return std::pair<int, int>(m_currChunkForTexUpdate - m_texUpdateRate, m_texUpdateRate);
}

bool Renderer::getRenderCompleteStatus() const noexcept
{
    if (!m_texUpdateLatch->try_wait()) return false;
    bool isRenderComplete = std::all_of(m_renderingStatusFutureVec.begin(), m_renderingStatusFutureVec.end(), [](const std::future<void>& fut) {
		return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	});

    return isRenderComplete;
}

Ray Renderer::getRayForPixel(int i, int currentRowCount) const noexcept
{
    const auto localCamProps{ m_mainCamera.getCameraProperties() };
    
    auto sampleOffset{ Vec3((UGenRNG<double>() - 0.5), (UGenRNG<double>() - 0.5), 0) };
    
    const Point currentRayForPixel{ localCamProps.camPixelDimObj.pixelCenter + ((i + sampleOffset.getX()) * localCamProps.camPixelDimObj.lateralSpanInAbsVal) + ((currentRowCount + sampleOffset.getY()) * localCamProps.camPixelDimObj.verticalSpanInAbsVal) };
    return Ray(localCamProps.camCenter, currentRayForPixel - localCamProps.camCenter);
}