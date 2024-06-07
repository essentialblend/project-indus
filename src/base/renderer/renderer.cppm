import renderer;

import <algorithm>;
import <numeric>;
import <exception>;
import <memory>;

import core_util;
import color;
import color_rgb;
import threadpool;
import hit_record;
import material;
import lambertian;
import world_object;


import <SFML/Graphics.hpp>;

std::unique_ptr<const IColor> Renderer::computeRayColor(const Ray& inputRay, const WorldObject& mainWorld, int maxRayBounceDepth)
{
    if (maxRayBounceDepth <= 0)
    {
        return createDerivedColorUniquePtr(m_renderColorType, Vec3(0));
    }

    HitRecord tempHitRecord{};
    if (mainWorld.checkHit(inputRay, Interval(0.001, +UInfinity), tempHitRecord))
    {
        Ray scatteredRay{};
        auto materialAlbedo{ createDerivedColorUniquePtr(m_renderColorType, Vec3(0)) };

        if (tempHitRecord.hitMaterial->handleRayScatter(inputRay, tempHitRecord, *materialAlbedo, scatteredRay))
        {
            materialAlbedo->multiplyColorWithSelf(*(computeRayColor(scatteredRay, mainWorld, maxRayBounceDepth - 1)));
            return materialAlbedo;
        }
    }

    return getBackgroundGradient(inputRay);
}

std::unique_ptr<const IColor> Renderer::getBackgroundGradient(const Ray& inputRay)
{
    const Vec3 gradientColorVec{ 0.5, 0.7, 1.0 };
    const Vec3 inputRayDir{ getUnit(inputRay.getDirection()) };
    const double lerpFactor{ 0.5 * (inputRayDir[1] + 1.0)};
    const Vec3 returnedColor{ ((1.0 - lerpFactor) * Vec3(1.0) + lerpFactor * gradientColorVec) };

    return createDerivedColorUniquePtr(m_renderColorType, returnedColor);
}

void Renderer::setupRenderer(const PixelResolution& pixResObj, const AspectRatio& aspectRatioObj)
{
    m_mainCamera = Camera{ pixResObj, aspectRatioObj };
    m_mainCamera.setupCamera();
    
    auto localPixDimObj{ m_mainCamera.getCameraProperties().camPixelDimObj };
    m_mainRenderingPassFutureVec.reserve(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);
    m_texUpdateLatch = std::make_unique<std::latch>(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);

    setupGaussianKernel(localPixDimObj);
}

void Renderer::setupGaussianKernel(PixelDimensionOLD& localPixDimObj)
{
    const Vec3 adjacentPixelForDist{ localPixDimObj.topLeftPixCenter + (1 * localPixDimObj.lateralSpanAbsVal) + (1 * localPixDimObj.vertSpanAbsVal) };
    localPixDimObj.pixelUnitSpanAbsVal = (localPixDimObj.topLeftPixCenter - adjacentPixelForDist).getMagnitude();
    m_mainCamera.setPixelDimensions(localPixDimObj);
    m_gaussianKernelProps.sigmaInAbsVal = 1.5 * localPixDimObj.pixelUnitSpanAbsVal;
    m_gaussianKernelProps.kernelSpanInIntegralVal = static_cast<int>((m_gaussianKernelProps.kernelCoverageScalar * m_gaussianKernelProps.sigmaInAbsVal) / localPixDimObj.pixelUnitSpanAbsVal);
    m_gaussianKernelProps.kernelWeights.resize(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.getTotalPixels());
}

GaussianKernelProperties Renderer::getGaussianKernelProps() const noexcept
{
    return m_gaussianKernelProps;
}

void Renderer::renderFrameMultiCoreGaussian(std::vector<std::unique_ptr<IColor>>& mainFramebuffer, const WorldObject& mainWorld)
{
    m_renderThreadPool.initiateThreadPool();

	for (int eachPixelRow{}; eachPixelRow < m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels; ++eachPixelRow)
    {
        m_mainRenderingPassFutureVec.push_back(m_renderThreadPool.enqueueThreadPoolTask([this, eachPixelRow, &mainFramebuffer, &mainWorld]() {
            this->renderPixelRowThreadPoolTaskGaussian(eachPixelRow, mainFramebuffer, mainWorld);
        }));
	}
}

void Renderer::setRendererSFMLFunctors(const RendererSFMLFunctors& rendererFuncObj) noexcept
{
    m_rendererFunctors = rendererFuncObj;
}

void Renderer::renderPixelRowThreadPoolTaskGaussian(int currentRowCount, std::vector<std::unique_ptr<IColor>>& mainFramebuffer, const WorldObject& mainWorld)
{
    const auto localCamProps{ m_mainCamera.getCameraProperties() };
    const auto localPixResObj{ m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj };
    const auto localPixDimObj{ m_mainCamera.getCameraProperties().camPixelDimObj };

    m_texUpdateLatch->count_down();

    // For every pixel.
    for (std::size_t pixelInRow{}; pixelInRow < localPixResObj.widthInPixels; ++pixelInRow)
    {
        std::unordered_map<long long, std::pair<std::shared_ptr<IColor>, double>> neighborPixelsContribMap{};
        PointOLD currentSamplePointOutVar{};

        // Generate and sample stratified pixels/rays.
        for (int subPixelGridV{}; subPixelGridV < m_sppSqrtCeil; ++subPixelGridV)
        {
            for (int subPixelGridU{}; subPixelGridU < m_sppSqrtCeil; ++subPixelGridU)
            {
                auto currPixelSampleColor{ createDerivedColorSharedPtr(m_renderColorType, Vec3(0)) };
                const Ray currentPixelRay{ getStratifiedRayForPixel(static_cast<int>(pixelInRow), currentRowCount, subPixelGridU, subPixelGridV, currentSamplePointOutVar) };
                currPixelSampleColor->addColorToSelf(*computeRayColor(currentPixelRay, mainWorld, m_maxRayBounceDepth - 1));
                
                // Collect and store sample contributions for neighboring pixels weighted by the gaussian kernel, for this sample.
                collectNeighborPixelContrib(currentRowCount, pixelInRow, localPixResObj, localPixDimObj, currentSamplePointOutVar, neighborPixelsContribMap, currPixelSampleColor);
            }
        }

        // Write contributions to neighboring pixels.
        {
            std::scoped_lock<std::mutex> lock(m_framebufferMutex);

            for (auto& [pixelIndex, pair] : neighborPixelsContribMap) 
            {    
                auto& [sampleColor, gaussianWeight] = pair;
                if (mainFramebuffer[pixelIndex]) 
                {
                    mainFramebuffer[pixelIndex]->addColorToSelf(*sampleColor);
                    m_gaussianKernelProps.kernelWeights[pixelIndex] += gaussianWeight;
                }
                else 
                {
                    auto finalColor{ createDerivedColorUniquePtr(m_renderColorType, Vec3(0)) };
                    finalColor->setColor(*sampleColor);
                    mainFramebuffer[pixelIndex] = std::move(finalColor);
                    m_gaussianKernelProps.kernelWeights[pixelIndex] = gaussianWeight;
                }
            }
        }
    }
}

void Renderer::collectNeighborPixelContrib(int currentRowCount, size_t pixelInRow, const PixelResolution& localPixResObj, const PixelDimensionOLD& localPixDimObj, const PointOLD& currentSamplePointOutVar, std::unordered_map<long long, std::pair<std::shared_ptr<IColor>, double>>& neighborPixelsContribMap, const std::shared_ptr<IColor>& currPixelSampleColor)
{
    const auto& kernelDiam{ m_gaussianKernelProps.kernelSpanInIntegralVal };
    const auto& sigma{ m_gaussianKernelProps.sigmaInAbsVal };
    const auto& kernelScalar{ m_gaussianKernelProps.kernelCoverageScalar };

    for (int dv{ -kernelDiam }; dv <= kernelDiam; ++dv)
    {
        for (int du{ -kernelDiam }; du <= kernelDiam; ++du)
        {
            const int nv{ (currentRowCount + dv) };
            const int nu{ static_cast<int>(pixelInRow + du) };

            if (nv < 0 || nu < 0 || nv > (localPixResObj.heightInPixels - 1) || nu > (localPixResObj.widthInPixels - 1)) continue;

            const PointOLD neighborPixelCenter{ localPixDimObj.topLeftPixCenter + (nu * localPixDimObj.lateralSpanAbsVal) + (nv * localPixDimObj.vertSpanAbsVal) };

            const PointOLD pixelSamplePoint{ currentSamplePointOutVar };
            const double currSampleToNeighborPixDistSq{ (pixelSamplePoint - neighborPixelCenter).getMagnitudeSq() };

            if (currSampleToNeighborPixDistSq < ((kernelScalar * sigma) * (kernelScalar * sigma)))
            {
                const auto pixelIndex{ static_cast<long long>(nv * localPixResObj.widthInPixels) + nu };
                const double sampleGaussianWeight{ std::exp(-currSampleToNeighborPixDistSq / (2 * sigma * sigma)) };

                auto sampleColorCopy{ createDerivedColorSharedPtr(m_renderColorType, Vec3(0)) };
                sampleColorCopy->setColor(*currPixelSampleColor);
                sampleColorCopy->multiplyScalarWithSelf(sampleGaussianWeight);

                if (neighborPixelsContribMap.count(pixelIndex) > 0)
                {   
                    neighborPixelsContribMap[pixelIndex].first->addColorToSelf(*sampleColorCopy);
                    neighborPixelsContribMap[pixelIndex].second += sampleGaussianWeight;
                }
                else 
                {
                    neighborPixelsContribMap[pixelIndex].first = sampleColorCopy;
                    neighborPixelsContribMap[pixelIndex].second = sampleGaussianWeight;
                }
            }
        }
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
    if (m_mainRenderingPassFutureVec.size() != m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels) return false;
   
    // updateChunk spans 0 to m_texUpdateRateOut rows for one chunk. 
    static int updateChunkForRangeStartOut{ 0 };
    const auto& localCopy{ std::span<std::future<void>>{m_mainRenderingPassFutureVec.begin(), m_mainRenderingPassFutureVec.end()} };
    bool canDrawCurrChunk{ areFuturesReadyInRange(updateChunkForRangeStartOut, localCopy, m_texUpdateRateOut) };

    if(canDrawCurrChunk)
    {
        auto test{ m_gaussianKernelProps.kernelWeights };
        updateChunkForRangeStartOut += m_texUpdateRateOut;
        if (updateChunkForRangeStartOut >= localCopy.size()) updateChunkForRangeStartOut = 0;
    }

    return canDrawCurrChunk;
}

bool Renderer::getRenderCompleteStatus() noexcept
{
    if (!m_texUpdateLatch->try_wait()) return false;
    bool isRenderComplete = std::all_of(m_mainRenderingPassFutureVec.begin(), m_mainRenderingPassFutureVec.end(), [](const std::future<void>& fut) {
        return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        });

    // Temp, to move.
    m_renderThreadPool.stopThreadPool();

    return isRenderComplete;
}

Ray Renderer::getStratifiedRayForPixel(int i, int currentRowCount, int subPixelGridU, int subPixelGridV, PointOLD& currentSamplePoint) const noexcept
{
    const auto localCamProps{ m_mainCamera.getCameraProperties() };
    const double stratumWidth{ 1.0 / m_sppSqrtCeil };
    const double stratumHeight{ 1.0 / m_sppSqrtCeil };

    auto sampleOffset{ Vec3(((subPixelGridU + UGenRNG<double>()) * stratumWidth), ((subPixelGridV + UGenRNG<double>()) * stratumHeight), 0) };

    currentSamplePoint = localCamProps.camPixelDimObj.topLeftPixCenter + ((i + sampleOffset[0]) * localCamProps.camPixelDimObj.lateralSpanAbsVal) + ((currentRowCount + sampleOffset[1]) * localCamProps.camPixelDimObj.vertSpanAbsVal);

    const auto randUnitDiskVec{ genRandomUnitDiskVec() };
    const auto randRayOriginOnUnitDisk{ localCamProps.camCenter + (randUnitDiskVec[0] * localCamProps.camDefocusPropsObj.defocusDiskU) + (randUnitDiskVec[1] * localCamProps.camDefocusPropsObj.defocusDiskV) };

    const Vec3 rayOrigin{ localCamProps.camDefocusPropsObj.defocusAngle <= 0 ? localCamProps.camCenter : randRayOriginOnUnitDisk };

    return Ray(rayOrigin, currentSamplePoint - rayOrigin);
}

int Renderer::getTexUpdateRate() const noexcept
{
    return m_texUpdateRateOut;
}

bool Renderer::areFuturesReadyInRange(int startOffset, std::span<std::future<void>> iterableFutureContainer, int& optExistingTracker)
{
    int numElemToIter{ std::min(optExistingTracker, static_cast<int>(iterableFutureContainer.size()) - startOffset) };
    optExistingTracker = numElemToIter;

    const auto subSpan{ iterableFutureContainer.subspan(startOffset, numElemToIter) };

    const bool isReady = std::all_of(subSpan.begin(), subSpan.end(), [](const std::future<void>& fut) {
        return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        });

    return isReady;
}

CameraProperties Renderer::getRendererCameraProps() const noexcept
{
    return m_mainCamera.getCameraProperties();
}

std::string Renderer::getRenderColorType() const noexcept
{
    return m_renderColorType;
}

std::unique_ptr<IColor> Renderer::createDerivedColorUniquePtr(const std::string& colorType, const Vec3& value) const
{
    if (colorType == "ColorRGB")
    {
        return std::make_unique<ColorRGB>(value);
    }
    else
    {
        throw std::runtime_error("Invalid color type specified.");
    }
}

std::shared_ptr<IColor> Renderer::createDerivedColorSharedPtr(const std::string& colorType, const Vec3& value) const
{
    if (colorType == "ColorRGB")
    {
        return std::make_shared<ColorRGB>(value);
    }
    else
    {
        throw std::runtime_error("Invalid color type specified.");
    }
}
