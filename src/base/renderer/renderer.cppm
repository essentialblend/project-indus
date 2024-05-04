import renderer;

import <algorithm>;
import <numeric>;

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

    m_gatherPixelSamplesPassFutureVec.reserve(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);

    m_mainRenderingPassFutureVec.reserve(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);
    
    m_texUpdateLatch = std::make_unique<std::latch>(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);
    
    m_pixelSamplesPassLatch = std::make_unique<std::latch>(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);

}

void Renderer::renderFrameMultiCore(std::vector<Color>& mainFramebuffer, const WorldObject& mainWorld)
{
    for (int eachPixelRow{}; eachPixelRow < m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels; ++eachPixelRow)
    {
        m_mainRenderingPassFutureVec.push_back(m_renderThreadPool.enqueueThreadPoolTask([this, eachPixelRow, &mainFramebuffer]() {
            this->renderPixelRowThreadPoolTaskGaussian(eachPixelRow, mainFramebuffer);
        }));    
    }
 }

void Renderer::samplesCollectionRenderPassMultiCore(const WorldObject& mainWorld)
{
    m_renderThreadPool.initiateThreadPool();
    for (int eachPixelRow{}; eachPixelRow < m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels; ++eachPixelRow)
    {
        m_gatherPixelSamplesPassFutureVec.push_back(m_renderThreadPool.enqueueThreadPoolTask([this, eachPixelRow, &mainWorld]() {
            this->collectPixelSamplesRowThreadPoolTask(eachPixelRow, mainWorld);
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

void Renderer::renderPixelRowThreadPoolTaskGaussian(int currentRowCount, std::vector<Color>& mainFramebuffer)
{
    const auto localCamProps{ m_mainCamera.getCameraProperties() };
    const auto localPixResObj{ m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj };
    const auto localPixDimObj{ m_mainCamera.getCameraProperties().camPixelDimObj };

    // We use the distance between two adjacent pixel centers as one pixel unit for the gaussian kernel.
    const Vec3 adjacentPixelForDist{ localPixDimObj.topLeftmostPixelCenter + (1 * localPixDimObj.lateralSpanInAbsVal) + (1 * localPixDimObj.verticalSpanInAbsVal) };
    const double pixelUnit{ (localPixDimObj.topLeftmostPixelCenter - adjacentPixelForDist).getMagnitude() };
    const double sigma{ 3 * pixelUnit };
    const int gaussianPixCoverage{ static_cast<int>((3 * sigma) / pixelUnit) };

    m_texUpdateLatch->count_down();

    //// For each pixel in row...
    //for (int i{}; i < localPixResObj.widthInPixels; ++i)
    //{
    //    int index{};
    //    Color normCurrPixColor(0);

    //    for (std::size_t dv{ static_cast<std::size_t>(-gaussianPixCoverage) }; dv <= (gaussianPixCoverage); ++dv)
    //    {
    //        for (std::size_t du{ static_cast<std::size_t>(-gaussianPixCoverage) }; du <= (gaussianPixCoverage); ++du)
    //        {
    //            const int nv{ static_cast<int>(currentRowCount + dv) };
    //            const int nu{ static_cast<int>(i + du) };

    //            if (nv < 0 || nu < 0 || nv > (localPixResObj.heightInPixels - 1) || nu >(localPixResObj.widthInPixels - 1)) continue;

    //            index = nu + (nv * localPixResObj.widthInPixels);

    //            const Point currentPixelCenter{ localPixDimObj.topLeftmostPixelCenter + (nu * localPixDimObj.lateralSpanInAbsVal) + (nv * localPixDimObj.verticalSpanInAbsVal) };
    //        }

    //    }
        //// For every pixel sample.
        //for (int subPixelGridV{}; subPixelGridV < m_sppSqrtFloored; ++subPixelGridV)
        //{
        //    std::vector<int> contribPix1DIndices{};
        //    std::vector<double> contribSampleWeights{};
        //    Point currentSamplePointOutVar{};

        //    for (int subPixelGridU{}; subPixelGridU < m_sppSqrtFloored; ++subPixelGridU)
        //    {
        //        const Ray currentPixelRay = getStratifiedRayForPixel(i, currentRowCount, subPixelGridU, subPixelGridV, currentSamplePointOutVar);
        //        normCurrPixColor = normCurrPixColor + computeRayColor(currentPixelRay, mainWorld, m_maxRayBounceDepth - 1);

        //        for (int dv{ -gaussianPixCoverage + 1 }; dv < gaussianPixCoverage; ++dv)
        //        {
        //            for (int du{ -gaussianPixCoverage + 1 }; du < gaussianPixCoverage; ++du)
        //            {
        //                const int nv{ currentRowCount + dv };
        //                const int nu{ i + du };

        //                if (nu < 0 || nv < 0 || nu >(localPixResObj.widthInPixels - 1) || nv >(localPixResObj.heightInPixels - 1)) continue;

        //                const Point neighborPixCentre{ currentPixelCenter + (nu * localPixDimObj.lateralSpanInAbsVal) + (nv * localPixDimObj.verticalSpanInAbsVal) };
        //                const double currSampleToNeighborPixDistSq{ (currentSamplePointOutVar - neighborPixCentre).getMagnitudeSq() };

        //                if (currSampleToNeighborPixDistSq > ((3 * sigma) * (3 * sigma))) continue;

        //                const double gaussianWeightForSampleContrib{ std::exp(-currSampleToNeighborPixDistSq / (2 * sigma * sigma)) };
        //            }
        //        }

        //    }

        //}
    //}
}

void Renderer::collectPixelSamplesRowThreadPoolTask(int currentRowCount, const WorldObject& mainWorld)
{
	const auto localCamProps{ m_mainCamera.getCameraProperties() };
	const auto localPixResObj{ m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj };
	const auto localPixDimObj{ m_mainCamera.getCameraProperties().camPixelDimObj };
    m_pixelSamplesPassLatch->count_down();
    // For each pixel in row...
    for (int i{}; i < localPixResObj.widthInPixels; ++i)
    {
        Color normCurrPixColor(0);
        const Point currentPixelCenter{ localPixDimObj.topLeftmostPixelCenter + (i * localPixDimObj.lateralSpanInAbsVal) + (currentRowCount * localPixDimObj.verticalSpanInAbsVal) };
        PixelSamples currentPixelSamples{ .pixel1DIndex{(currentRowCount * localPixResObj.widthInPixels) + i} };
        
        // For every pixel sample.
        for (int subPixelGridV{}; subPixelGridV < m_sppSqrtFloored; ++subPixelGridV)
        {
            std::vector<int> contribPix1DIndices{};
            std::vector<double> contribSampleWeights{};
            Point currentSamplePointOutVar{};

            for (int subPixelGridU{}; subPixelGridU < m_sppSqrtFloored; ++subPixelGridU)
            {
                const Ray currentPixelRay = getStratifiedRayForPixel(i, currentRowCount, subPixelGridU, subPixelGridV, currentSamplePointOutVar);
                normCurrPixColor = normCurrPixColor + computeRayColor(currentPixelRay, mainWorld, m_maxRayBounceDepth - 1);

                currentPixelSamples.pixelSamples.emplace_back(currentSamplePointOutVar, normCurrPixColor);
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
    if (m_gatherPixelSamplesPassFutureVec.size() != m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels) return false;
    
    // updateChunk spans 0 to m_texUpdateRate rows for one chunk. 
    static int updateChunk{ 0 };
    
    const auto itBegin{ m_gatherPixelSamplesPassFutureVec.begin() + updateChunk };
    const auto numElementsRemaining = std::distance(itBegin, m_gatherPixelSamplesPassFutureVec.end());

    if (numElementsRemaining < m_texUpdateRate)
    {
        m_texUpdateRate = static_cast<int>(numElementsRemaining);
    }

    const auto itEnd{ itBegin + m_texUpdateRate };

    bool canCurrentChunkBeDrawn = std::all_of(itBegin, itEnd, [](const std::future<void>& fut) {
		return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	});

    if (itEnd == m_gatherPixelSamplesPassFutureVec.end() && canCurrentChunkBeDrawn)
    {
        updateChunk = 0;
    }
    else if(canCurrentChunkBeDrawn)
    {
        updateChunk += m_texUpdateRate;
    }

    return canCurrentChunkBeDrawn;
}

bool Renderer::getSampleCollectionPassCompleteStatus() noexcept
{
    if (!m_pixelSamplesPassLatch->try_wait()) return false;
    bool isSampleCollectionComplete = std::all_of(m_gatherPixelSamplesPassFutureVec.begin(), m_gatherPixelSamplesPassFutureVec.end(), [](const std::future<void>& fut) {
        return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    });

    return isSampleCollectionComplete;
}

bool Renderer::getRenderCompleteStatus() noexcept
{
    if (!m_texUpdateLatch->try_wait()) return false;
    bool isRenderComplete = std::all_of(m_mainRenderingPassFutureVec.begin(), m_mainRenderingPassFutureVec.end(), [](const std::future<void>& fut) {
        return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        });

    if (isRenderComplete)
    {
        m_renderThreadPool.stopThreadPool();
    }

    return isRenderComplete;
}

Ray Renderer::getRayForPixel(int i, int currentRowCount) const noexcept
{
    const auto localCamProps{ m_mainCamera.getCameraProperties() };
    
    auto sampleOffset{ Vec3((UGenRNG<double>() - 0.5), (UGenRNG<double>() - 0.5), 0) };
    
    const Point currentRayForPixel{ localCamProps.camPixelDimObj.topLeftmostPixelCenter + ((i + sampleOffset.getX()) * localCamProps.camPixelDimObj.lateralSpanInAbsVal) + ((currentRowCount + sampleOffset.getY()) * localCamProps.camPixelDimObj.verticalSpanInAbsVal) };

    const auto randUnitDiskVec{ genRandomUnitDiskVec() };
    const auto randRayOriginOnUnitDisk{ localCamProps.camCenter + (randUnitDiskVec.getX() * localCamProps.defocusDiskU) + (randUnitDiskVec.getY() * localCamProps.defocusDiskV) };

    const Vec3 rayOrigin{ localCamProps.defocusAngle <= 0 ? localCamProps.camCenter : randRayOriginOnUnitDisk };

    return Ray(rayOrigin, currentRayForPixel - rayOrigin);
}

Ray Renderer::getStratifiedRayForPixel(int i, int currentRowCount, int subPixelGridU, int subPixelGridV, Point& currentSamplePoint) const noexcept
{
    const auto localCamProps{ m_mainCamera.getCameraProperties() };
    const double stratumWidth{ 1.0 / m_sppSqrtFloored };
    const double stratumHeight{ 1.0 / m_sppSqrtFloored };

    auto sampleOffset{ Vec3(((subPixelGridU + UGenRNG<double>()) * stratumWidth), ((subPixelGridV + UGenRNG<double>()) * stratumHeight), 0) };

    currentSamplePoint = localCamProps.camPixelDimObj.topLeftmostPixelCenter + ((i + sampleOffset.getX()) * localCamProps.camPixelDimObj.lateralSpanInAbsVal) + ((currentRowCount + sampleOffset.getY()) * localCamProps.camPixelDimObj.verticalSpanInAbsVal);

    const auto randUnitDiskVec{ genRandomUnitDiskVec() };
    const auto randRayOriginOnUnitDisk{ localCamProps.camCenter + (randUnitDiskVec.getX() * localCamProps.defocusDiskU) + (randUnitDiskVec.getY() * localCamProps.defocusDiskV) };

    const Vec3 rayOrigin{ localCamProps.defocusAngle <= 0 ? localCamProps.camCenter : randRayOriginOnUnitDisk };

    return Ray(rayOrigin, currentSamplePoint - rayOrigin);
}

int Renderer::getTexUpdateRate() const noexcept
{
    return m_texUpdateRate;
}