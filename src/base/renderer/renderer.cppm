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
        // Return black (RGB: 0, 0, 0).
        return createDerivedTypeUniquePtr(m_renderColorType, Vec3(0));
    }

    HitRecord tempHitRecord{};
    
    if (mainWorld.checkHit(inputRay, Interval(0.001, +UInfinity), tempHitRecord))
    {
        Ray scatteredRay{};

        auto materialAlbedo{ createDerivedTypeUniquePtr(m_renderColorType, Vec3(0)) };

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

    return createDerivedTypeUniquePtr(m_renderColorType, returnedColor);
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

void Renderer::setupGaussianKernel(PixelDimension& localPixDimObj)
{
    const Vec3 adjacentPixelForDist{ localPixDimObj.topLeftmostPixelCenter + (1 * localPixDimObj.lateralSpanInAbsVal) + (1 * localPixDimObj.verticalSpanInAbsVal) };
    localPixDimObj.pixelUnitSpanInAbsVal = (localPixDimObj.topLeftmostPixelCenter - adjacentPixelForDist).getMagnitude();
    m_mainCamera.setPixelDimensions(localPixDimObj);
    m_gaussianKernelProps.sigmaInAbsVal = 0.25 * localPixDimObj.pixelUnitSpanInAbsVal;
    m_gaussianKernelProps.kernelSpanInIntegralVal = static_cast<int>((m_gaussianKernelProps.kernelCoverageScalar * m_gaussianKernelProps.sigmaInAbsVal) / localPixDimObj.pixelUnitSpanInAbsVal);
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

    for (std::size_t pixelInRow{}; pixelInRow < localPixResObj.widthInPixels; ++pixelInRow)
    {
        Point currentSamplePointOutVar{};
        auto currPixelSampleColor{ createDerivedTypeUniquePtr(m_renderColorType, Vec3(0)) };

        for (int subPixelGridV{}; subPixelGridV < m_sppSqrtCeil; ++subPixelGridV)
        {
            for (int subPixelGridU{}; subPixelGridU < m_sppSqrtCeil; ++subPixelGridU)
            {
                const Ray currentPixelRay = getStratifiedRayForPixel(static_cast<int>(pixelInRow), currentRowCount, subPixelGridU, subPixelGridV, currentSamplePointOutVar);

                currPixelSampleColor->addColorToSelf(*computeRayColor(currentPixelRay, mainWorld, m_maxRayBounceDepth - 1));
            }
        }
        currPixelSampleColor->applyWeights(m_sppSqrtCeil * m_sppSqrtCeil);

        mainFramebuffer[static_cast<long long>(currentRowCount * localPixResObj.widthInPixels) + pixelInRow] = std::move(currPixelSampleColor);
    }
}

//void Renderer::accumNeighborPixelSamplesGaussianContrib(const GaussianKernelProperties& gaussKernelPropsObj, int currentRowCount, size_t pixelInRow, const PixelResolution& localPixResObj, const PixelDimension& localPixDimObj, double& accumWeightForPixelColor, Color& normalizedPixelColor)
//{
//    const auto& sigma{ gaussKernelPropsObj.sigmaInAbsVal };
//    const auto& kernelDiam{ gaussKernelPropsObj.kernelSpanInIntegralVal };
//    const auto& kernelScalar{ gaussKernelPropsObj.kernelCoverageScalar };
//
//    std::vector<PixelSampleData> localNeighborPixelSamplesVec{};
//
//    for (int dv{ (-kernelDiam) }; dv <= kernelDiam; ++dv)
//    {
//        for (int du{ (-kernelDiam) }; du <= kernelDiam; ++du)
//        {
//            const int nv{ (currentRowCount + dv) };
//            const int nu{ static_cast<int>(pixelInRow + du) };
//
//            if (nv < 0 || nu < 0 || nv >(localPixResObj.heightInPixels - 1) || nu >(localPixResObj.widthInPixels - 1)) continue;
//
//            const Point neighborPixelCenter{ localPixDimObj.topLeftmostPixelCenter + ((nu) * localPixDimObj.lateralSpanInAbsVal) + ((nv) * localPixDimObj.verticalSpanInAbsVal) };
//
//            {
//                std::scoped_lock emplaceLock{ m_mapReadWriteMutex };
//                localNeighborPixelSamplesVec = m_pixelSamplesMap[static_cast<long long>(nv * localPixResObj.widthInPixels) + nu].pixelSamples;
//            }
//
//            std::for_each(localNeighborPixelSamplesVec.begin(), localNeighborPixelSamplesVec.end(), [&](PixelSampleData sample) {
//
//                const Point pixelSamplePoint{ sample.pointOnViewPlane };
//                const double currSampleToNeighborPixDistSq{ (pixelSamplePoint - neighborPixelCenter).getMagnitudeSq() };
//
//                if (currSampleToNeighborPixDistSq < ((kernelScalar * sigma) * (kernelScalar * sigma)))
//                {
//                    const double gaussianWeightForSampleContrib{ std::exp(-currSampleToNeighborPixDistSq / (2 * sigma * sigma)) };
//                    accumWeightForPixelColor += gaussianWeightForSampleContrib;
//                    normalizedPixelColor = normalizedPixelColor + (sample.computedBaseColor * gaussianWeightForSampleContrib);
//                }
//            });
//        }
//    }
//}

//void Renderer::collectPixelSamplesRowThreadPoolTask(int currentRowCount, const WorldObject& mainWorld)
//{
//	const auto localCamProps{ m_mainCamera.getCameraProperties() };
//	const auto localPixResObj{ m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj };
//	const auto localPixDimObj{ m_mainCamera.getCameraProperties().camPixelDimObj };
//    m_pixelSamplesPassLatch->count_down();
//    // For each pixel in row...
//    for (int i{}; i < localPixResObj.widthInPixels; ++i)
//    {
//        const Point currentPixelCenter{ localPixDimObj.topLeftmostPixelCenter + (i * localPixDimObj.lateralSpanInAbsVal) + (currentRowCount * localPixDimObj.verticalSpanInAbsVal) };
//        PixelSamples currentPixelSamples{};
//        currentPixelSamples.pixelSamples.reserve(static_cast<long long>(m_sppSqrtCeil * m_sppSqrtCeil));
//        
//        // For every pixel sample.
//        for (int subPixelGridV{}; subPixelGridV < m_sppSqrtCeil; ++subPixelGridV)
//        {
//            Point currentSamplePointOutVar{};
//
//            for (int subPixelGridU{}; subPixelGridU < m_sppSqrtCeil; ++subPixelGridU)
//            {
//                Color normCurrPixColor(0);
//                const Ray currentPixelRay = getStratifiedRayForPixel(i, currentRowCount, subPixelGridU, subPixelGridV, currentSamplePointOutVar);
//                normCurrPixColor = normCurrPixColor + computeRayColor(currentPixelRay, mainWorld, m_maxRayBounceDepth - 1);
//
//                currentPixelSamples.pixelSamples.emplace_back(PixelSampleData(currentSamplePointOutVar, normCurrPixColor));
//            }
//        }
//
//        {
//            std::scoped_lock emplaceLock{ m_mapReadWriteMutex };
//
//            m_pixelSamplesMap.emplace(std::pair<std::size_t, PixelSamples>(static_cast<std::size_t>((currentRowCount * localPixResObj.widthInPixels) + i), currentPixelSamples));
//        }
//    }
//}

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

    return isRenderComplete;
}

Ray Renderer::getStratifiedRayForPixel(int i, int currentRowCount, int subPixelGridU, int subPixelGridV, Point& currentSamplePoint) const noexcept
{
    const auto localCamProps{ m_mainCamera.getCameraProperties() };
    const double stratumWidth{ 1.0 / m_sppSqrtCeil };
    const double stratumHeight{ 1.0 / m_sppSqrtCeil };

    auto sampleOffset{ Vec3(((subPixelGridU + UGenRNG<double>()) * stratumWidth), ((subPixelGridV + UGenRNG<double>()) * stratumHeight), 0) };

    currentSamplePoint = localCamProps.camPixelDimObj.topLeftmostPixelCenter + ((i + sampleOffset[0]) * localCamProps.camPixelDimObj.lateralSpanInAbsVal) + ((currentRowCount + sampleOffset[1]) * localCamProps.camPixelDimObj.verticalSpanInAbsVal);

    const auto randUnitDiskVec{ genRandomUnitDiskVec() };
    const auto randRayOriginOnUnitDisk{ localCamProps.camCenter + (randUnitDiskVec[0] * localCamProps.defocusDiskU) + (randUnitDiskVec[1] * localCamProps.defocusDiskV) };

    const Vec3 rayOrigin{ localCamProps.defocusAngle <= 0 ? localCamProps.camCenter : randRayOriginOnUnitDisk };

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

std::unique_ptr<IColor> Renderer::createDerivedTypeUniquePtr(const std::string& colorType, const Vec3& value) const
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