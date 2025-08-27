import renderer;

import <algorithm>;
import <numeric>;
import <exception>;
import <memory>;

import core_util;
import core_diag;
import core_sampling_util;
import threadpool;
import hit_record;
import material;
import matte;
import world_object;
import vec3;
import bxdf;
import randomsampler;
import stratifiedsampler;

ColorRGB Renderer::Li(const Ray& inputRayWithUnitDir, const WorldObject& mainWorld, int maxRayBounceDepth, Sampler& sampler)
{
  ColorRGB L{ 0 };                     
  ColorRGB beta{ 1 };
  Ray ray{ inputRayWithUnitDir };

  if constexpr (diagCompileFlag) diagCount(m_totalPaths);

  for (int bounce = 0; bounce < maxRayBounceDepth; ++bounce)
  {
    if constexpr (diagCompileFlag) diagCount(m_totalBounces);

    HitRecord hit{};
    if (!mainWorld.checkHit(ray, Interval(0.0001, UInfinity), hit))
    {
      L += beta * getBackgroundGradient(ray);
      break;
    }

    if (hit.hitMaterial->getMaterialType() == MaterialType::Matte) 
    {
      //__debugbreak();
    }

    hit.hitMaterial->computeScatteringFunctions(hit);

    if (!hit.surfaceBSDF)
    {
      if constexpr (diagCompileFlag) diagCount(m_samplesRejected);
      break;
    }

    auto [unitW_i, PDFVal, BRDFVal, bxdfType] = hit.surfaceBSDF->sample(-ray.getDirection(), sampler.get2D());

    if (!(PDFVal > 0.0) || !std::isfinite(PDFVal) || !isFiniteVec(BRDFVal))
    {
      if constexpr (diagCompileFlag) diagCount(m_samplesRejected);
      break;
    }

    if (isSpecularBxDF(bxdfType))
    {
      beta *= BRDFVal;
    }
    else
    {
      const double cosTheta{ std::max(0.0, computeDot(unitW_i, hit.shadingBasis.m_unitNormalVec)) };
      beta *= (BRDFVal * (cosTheta / PDFVal));
    }

    if (m_useRussianRoulette && bounce >= 5)
    {
      double q{ std::min(0.95, std::max({beta[0], beta[1], beta[2]})) };

      if (sampler.get1D() > q)
      {
        if constexpr (diagCompileFlag) diagCount(m_pathsTerminated);
        break;
      }

      beta *= (1.0 / q);
      if constexpr (diagCompileFlag) diagCount(m_pathsSurvived);
    }

    ray = Ray{ hit.hitPoint, unitW_i };
  }

  return L;
}

ColorRGB Renderer::getBackgroundGradient(const Ray& inputRay)
{
  const ColorRGB gradientColorVec{ 0.5, 0.7, 1.0 };
  
  const Vec3 inputRayDir{ getUnit(inputRay.getDirection()) };
  const double lerpFactor{ 0.75 * (inputRayDir[1] + 1.0)};
  const ColorRGB returnedColor{ ((1.0 - lerpFactor) * Vec3(1.0) + lerpFactor * gradientColorVec) };

  return returnedColor;
}

void Renderer::setupRenderer(const PixelResolution& pixResObj, const AspectRatio& aspectRatioObj)
{
    // Setup camera resolution and aspect ratio
    m_mainCamera = Camera{ pixResObj, aspectRatioObj };
    m_mainCamera.setupCamera();
    
    // Setup pixel dimensions
    auto localPixDimObj{ m_mainCamera.getCameraProperties().camPixelDimObj };

    // Reserve a vector of std::futures representing each row of pixels which constitute a rendering task for the threadpool
    m_mainRenderingPassFutureVec.reserve(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);
    
    // Setup the latch
    m_texUpdateLatch = std::make_unique<std::latch>(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels);

    if constexpr (diagCompileFlag)
    {
      m_pixelsFlagged.store(0, std::memory_order_relaxed);
      m_diagPrinted = false;
      Renderer::m_samplesRejected = false;
    }

    // Setup the sampler
    if (m_isMultithreaded)
    {
      int nx{ static_cast<int>(std::floor(std::sqrt(m_SPP))) };
      int ny{ (m_SPP + nx - 1) / nx };
      m_rendererSampler = std::make_unique<StratifiedSampler>(nx, ny, true, m_samplerSeed);
    }
    else
    {
      m_rendererSampler = std::make_unique<StratifiedSampler>(1, 1, true, m_samplerSeed);
    }

    //setupGaussianKernel(localPixDimObj);
}

void Renderer::setupGaussianKernel(PixelDimension& localPixDimObj)
{
    const Vec3 adjacentPixelForDist{ localPixDimObj.topLeftPixCenter + (1 * localPixDimObj.lateralSpanVec) + (1 * localPixDimObj.vertSpanVec) };

    localPixDimObj.pixelUnitSpanAbsVal = (localPixDimObj.topLeftPixCenter - adjacentPixelForDist).getMagnitude();
    
    m_mainCamera.setPixelDimensions(localPixDimObj);
    
    m_gaussianKernelProps.sigmaInAbsVal = 0.75 * localPixDimObj.pixelUnitSpanAbsVal;
    
    m_gaussianKernelProps.kernelSpanInIntegralVal = static_cast<int>((m_gaussianKernelProps.kernelCoverageScalar * 
    m_gaussianKernelProps.sigmaInAbsVal) / localPixDimObj.pixelUnitSpanAbsVal);
    
    m_gaussianKernelProps.kernelWeights.resize(m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.getTotalPixels());
}

GaussianKernelProperties Renderer::getGaussianKernelProps() const noexcept
{
    return m_gaussianKernelProps;
}

void Renderer::renderFrame(std::vector<ColorRGB>& mainFramebuffer, const WorldObject& mainWorld, bool isMultithreaded)
{
  // Begin main render timer log.
  m_mainRenderTimer.resetTimer();
  m_mainRenderTimer.startTimer();

  if (isMultithreaded)
  {
    m_renderThreadPool.initiateThreadPool(std::thread::hardware_concurrency());
  }
  else
  {
    m_renderThreadPool.initiateThreadPool();
  }

	for (int eachPixelRow{}; eachPixelRow < m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.heightInPixels; ++eachPixelRow)
  {
    m_mainRenderingPassFutureVec.push_back(m_renderThreadPool.enqueueThreadPoolTask([this, eachPixelRow, &mainFramebuffer, &mainWorld]()
      {
        this->renderPixelRowThreadPoolTask(eachPixelRow, mainFramebuffer, mainWorld);
      }));
	}
}

void Renderer::renderLiTestFrame(std::vector<ColorRGB>& mainFramebuffer, const WorldObject& mainWorld, bool isMultithreaded)
{
  const int spp = 10;       
  const int maxDepth = 5;

  const auto& camProps = m_mainCamera.getCameraProperties();
  int cx = camProps.camImgPropsObj.pixelResolutionObj.widthInPixels / 2;
  int cy = camProps.camImgPropsObj.pixelResolutionObj.heightInPixels / 2;

  Ray ray = m_mainCamera.generateRay(cx, cy, { 0.5,0.5 }, { 0.5,0.5 });
  std::println("Ray origin = ({}, {}, {}), dir = ({}, {}, {})",
    ray.getOrigin()[0], ray.getOrigin()[1], ray.getOrigin()[2],
    ray.getDirection()[0], ray.getDirection()[1], ray.getDirection()[2]);

  HitRecord hit{};
  bool ok = mainWorld.checkHit(ray, Interval(0.0001, UInfinity), hit);
  std::println("checkHit = {}", ok);
  if (ok) {
    std::println("Hit point = ({}, {}, {}), normal = ({}, {}, {})",
      hit.hitPoint[0], hit.hitPoint[1], hit.hitPoint[2],
      hit.shadingBasis.m_unitNormalVec[0],
      hit.shadingBasis.m_unitNormalVec[1],
      hit.shadingBasis.m_unitNormalVec[2]);
  }

  // run Li() a few samples
  ColorRGB accum{ 0 };
  auto sampler = m_rendererSampler->clone(m_samplerSeed);
  for (int i = 0; i < spp; ++i) {
    accum += Li(ray, mainWorld, maxDepth, *sampler);
    sampler->startNextSample();
  }
  accum *= (1.0 / spp);
  std::println("Li result = ({}, {}, {})", accum[0], accum[1], accum[2]);
}

void Renderer::setRendererSFMLFunctors(const RendererSFMLFunctors& rendererFuncObj) noexcept
{
    m_rendererFunctors = rendererFuncObj;
}

void Renderer::renderPixelRowThreadPoolTask(int currentRowCount, std::vector<ColorRGB>& mainFramebuffer, const WorldObject& mainWorld)
{
  const auto widthResPixels{ m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj.widthInPixels };
  
  const auto SPP{ m_rendererSampler->getSPP() };
  auto localSampler{ m_rendererSampler->clone(m_samplerSeed + currentRowCount) };

  // For every pixel
  for (std::size_t pixelInRow{}; pixelInRow < widthResPixels; ++pixelInRow)
  {
    if constexpr (diagCompileFlag)
    {
      Renderer::m_samplesRejected = false;
    }
    
    ColorRGB currPixelAccumColor{ 0 };
    
    localSampler->startPixelSample(static_cast<int>(pixelInRow), currentRowCount, 0);

    for (int currSample{}; currSample < SPP; ++currSample)
    {
      auto uPixel{ localSampler->get2D() };
      auto uLens{ localSampler->get2D() };
      
      Ray rayForSample{ m_mainCamera.generateRay(static_cast<int>(pixelInRow), currentRowCount, uPixel, uLens) };
      currPixelAccumColor += Li(rayForSample, mainWorld, m_maxRayBounceDepth, *localSampler);

      localSampler->startNextSample();
    }

    if constexpr (diagCompileFlag)
    {
      if (Renderer::m_samplesRejected) diagCount(m_pixelsFlagged);
    }

    // Normalize the accumulated color
    currPixelAccumColor *= (1.0 / SPP);
    
    // Store in the 1D serialized framebuffer
    std::size_t bufferIndex{ static_cast<std::size_t>((static_cast<long long>(currentRowCount) * widthResPixels) + pixelInRow) };
    mainFramebuffer[bufferIndex] = currPixelAccumColor;
  }

  m_texUpdateLatch->count_down();
}

//void Renderer::collectNeighborPixelContrib(int currentRowCount, size_t pixelInRow, const PixelResolution& localPixResObj, const PixelDimension& localPixDimObj, const Point& currentSamplePointOutVar, std::unordered_map<long long, std::pair<std::shared_ptr<IColor>, double>>& neighborPixelsContribMap, const std::shared_ptr<IColor>& currPixelSampleColor)
//{
//    const auto& kernelDiam{ m_gaussianKernelProps.kernelSpanInIntegralVal };
//    const auto& sigma{ m_gaussianKernelProps.sigmaInAbsVal };
//    const auto& kernelScalar{ m_gaussianKernelProps.kernelCoverageScalar };
//
//    for (int dv{ -kernelDiam }; dv <= kernelDiam; ++dv)
//    {
//        for (int du{ -kernelDiam }; du <= kernelDiam; ++du)
//        {
//            const int nv{ (currentRowCount + dv) };
//            const int nu{ static_cast<int>(pixelInRow + du) };
//
//            if (nv < 0 || nu < 0 || nv > (localPixResObj.heightInPixels - 1) || nu > (localPixResObj.widthInPixels - 1)) continue;
//
//            const Point neighborPixelCenter{ localPixDimObj.topLeftPixCenter + (nu * localPixDimObj.lateralSpanVec) + (nv * localPixDimObj.vertSpanVec) };
//
//            const Point pixelSamplePoint{ currentSamplePointOutVar };
//            const double currSampleToNeighborPixDistSq{ (pixelSamplePoint - neighborPixelCenter).getMagnitudeSq() };
//
//            if (currSampleToNeighborPixDistSq < ((kernelScalar * sigma) * (kernelScalar * sigma)))
//            {
//                const auto pixelIndex{ static_cast<long long>(nv * localPixResObj.widthInPixels) + nu };
//                const double sampleGaussianWeight{ std::exp(-currSampleToNeighborPixDistSq / (2 * sigma * sigma)) };
//
//                auto sampleColorCopy{ createDerivedColorSharedPtr(m_renderColorType, Vec3(0)) };
//                sampleColorCopy->setColor(*currPixelSampleColor);
//                sampleColorCopy->multiplyScalarWithSelf(sampleGaussianWeight);
//
//                if (neighborPixelsContribMap.count(pixelIndex) > 0)
//                {   
//                    neighborPixelsContribMap[pixelIndex].first->addColorToSelf(*sampleColorCopy);
//                    neighborPixelsContribMap[pixelIndex].second += sampleGaussianWeight;
//                }
//                else 
//                {
//                    neighborPixelsContribMap[pixelIndex].first = sampleColorCopy;
//                    neighborPixelsContribMap[pixelIndex].second = sampleGaussianWeight;
//                }
//            }
//        }
//    }
//}

void Renderer::setThreadingMode(bool isMultithreaded) noexcept
{
    m_isMultithreaded = isMultithreaded;
}

void Renderer::setSPP(int spp) noexcept
{
  m_SPP = spp;
}

void Renderer::setRayBounceDepth(int rayBounceDepth) noexcept
{
  m_maxRayBounceDepth = rayBounceDepth;
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

    if (isRenderComplete)
    {
      
      m_mainRenderTimer.stopTimer();
      
      const auto& localPixResObj{ m_mainCamera.getCameraProperties().camImgPropsObj.pixelResolutionObj };
      double elapsedSeconds{ m_mainRenderTimer.getElapsedTime().count() };
      
      std::size_t totalSampleCount{ static_cast<std::size_t>(localPixResObj.widthInPixels * localPixResObj.heightInPixels * m_SPP) };

      double sppRate{ totalSampleCount / elapsedSeconds };
      
      if constexpr (diagCompileFlag)
      {
        if (!m_diagPrinted && diagRunFlag.load())
        {
          const auto flagged = m_pixelsFlagged.load(std::memory_order_relaxed);
          double avgDepth{ static_cast<double>(m_totalBounces) / m_totalPaths };
          double benefit = 1.0 - (avgDepth / m_maxRayBounceDepth);

          std::println("[DIAG] PIXELS FLAGGED: {}\n", flagged);

          std::println("[DIAG] RR PATHS TERMINATED: {}", m_pathsTerminated.load(std::memory_order_relaxed));
          std::println("[DIAG] RR PATHS SURVIVED: {}", m_pathsSurvived.load(std::memory_order_relaxed));
          std::println("[DIAG] AVG DEPTH WITH RR: {:.2f}", avgDepth);
          std::println("[DIAG] SAVINGS VS MAXDEPTH {}: {:.1f}%\n",
            m_maxRayBounceDepth, benefit * 100.0);
          
          std::println("[PERF] Frame time: {:.2f}s, Throughput: {:.2f} samples/sec", elapsedSeconds, sppRate);
          
          m_diagPrinted = true;
        }
      }

      // To move
      m_renderThreadPool.stopThreadPool();
    }

    return isRenderComplete;
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


