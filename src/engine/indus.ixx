export module indus.engine;

import std;

import indus.core.math.constants.i;
import indus.core.math.fp.ii;
import indus.core.math.algebra.iv;

import indus.geom.sphere;
import indus.geom.bvh_aggregate;
import indus.geom.primitive;

import indus.film.base;
import indus.film.pixel;

import indus.camera.base;

import indus.integrator.base;
import indus.integrator.constructs;

export import indus.engine.constructs;
import indus.engine.factory;
import indus.engine.frame_mailbox;

import indus.shading.material;
import indus.shading.dielectric;
import indus.shading.diffuse;
import indus.shading.coated_diffuse;

import indus.rng.pcg32;

import indus.sampler.base;

import indus.stats.accumulator;

import indus.scene;

import indus.utilities.basictimer;
import indus.utilities.parallel;
import indus.utilities.threadpool;

import indus.ui.displaysink_base;

export class Indus final
{
public:
  explicit Indus(const IndusConfig& cfg) noexcept;

  void runEngine();

  DisplayConsumer createDisplayConsumer() noexcept;

private:
  IndusConfig m_cfg{};

  std::unique_ptr<FilmBase> m_film{};
  std::unique_ptr<CameraBase> m_camera{};
  std::unique_ptr<Sampler> m_sampler{};
  std::unique_ptr<Integrator> m_integrator{};
  std::unique_ptr<ThreadPool> m_engineThreadPool{};
  std::unique_ptr<DisplaySinkBase> m_displaySink{};
  std::unique_ptr<Scene> m_renderScene{};

  std::jthread m_renderThread{};

  FrameMailbox m_frameMailbox{};
  static inline EngineBuildInformation m_engineBuildInfo;

  BasicTimer m_HUDTimer{};

  std::shared_ptr<Primitive> makeShirleyBook1BVHRoot(const Transform4f& renderFromWorld, const Point2f& coatedXZ, const Point2f& glassXZ, bool enableMotionBlur);

  void initializeParallelSystems(std::size_t numThreads) noexcept;
  void setupEngine();
  void shutdownParallelSystems() noexcept;
  void renderScene();
  void runGUI();
};

Indus::Indus(const IndusConfig& cfg) noexcept : m_cfg{ cfg } {}

void Indus::setupEngine()
{
  auto& engineSystemsFactory{ EngineSystemsFactory::getInstance() };
  const auto& hardwareThreads{ std::thread::hardware_concurrency() };

  m_film = engineSystemsFactory.makeFilm(m_cfg.filmCfg);
  m_camera = engineSystemsFactory.makeCamera(m_cfg.camCfg, *m_film);
  m_sampler = engineSystemsFactory.makeSampler(m_cfg.samplerCfg);

  m_integrator = engineSystemsFactory.makeIntegrator(m_cfg.integratorCfg, *m_camera, *m_sampler);

  m_integrator->setDisplayConsumer(createDisplayConsumer());

  const Point2f matteBallPosition{ 0.75, -1.25 };
  const Point2f dielectricBallPosition{ 0, 0 };
  
  std::shared_ptr<Primitive> sceneRootAggregate{ makeShirleyBook1BVHRoot(m_camera->getCameraTransform().getRenderFromWorld(), matteBallPosition, dielectricBallPosition, true) };

  m_renderScene = std::make_unique<Scene>(sceneRootAggregate);

  m_engineBuildInfo = engineSystemsFactory.makeDefaultEngineBuildInformation();
  m_engineBuildInfo.runtimeThreads = hardwareThreads != 0u ? hardwareThreads : 1u;

  ImmutableEngineSystems immutables{ *m_film, *m_camera, *m_sampler, *m_integrator, *m_renderScene };

  m_displaySink = engineSystemsFactory.makeDisplaySink(immutables, m_cfg.displaySinkCfg, m_HUDTimer, m_engineBuildInfo, m_cfg.filmCfg);

  StatsAccumulator::setFilmBytes(static_cast<UInt64>(m_film->getFilmResolution()[0]) * (m_film->getFilmResolution()[1]) * sizeof(Pixel));
}

void Indus::runEngine()
{
  initializeParallelSystems(std::max(1u, std::thread::hardware_concurrency()));
  setupEngine();

  renderScene();
  runGUI();
}

void Indus::renderScene()
{
  m_renderThread = std::jthread([this](std::stop_token stopToken)
  {
    m_HUDTimer.startTimer();
    m_integrator->render(*m_renderScene, stopToken);
    m_HUDTimer.stopTimer();

    if (!stopToken.stop_requested())
      m_film->writeImage(m_cfg.samplerCfg.samplesPerPixel, m_HUDTimer);
  });
}

void Indus::runGUI()
{
  FrameSnapshot last{};
  UInt64 lastVersion{ 0 };

  while (m_displaySink->isSinkOpen())
  {
    if (std::optional<FrameSnapshot> snap{ m_frameMailbox.tryConsume() })
    {
      if (snap->frameVersion > lastVersion)
      {
        m_displaySink->update(*snap);
        last = *snap;
        lastVersion = snap->frameVersion;
      }
    }

    m_displaySink->present();
  }

  if (m_renderThread.joinable())
  {
    m_renderThread.request_stop();
    m_renderThread.join();
  }
}

DisplayConsumer Indus::createDisplayConsumer() noexcept
{
  return [this](FrameSnapshot frameSnapshot) 
  {
    m_frameMailbox.publishFrameSnapshot(std::move(frameSnapshot));
  };
}

std::shared_ptr<Primitive> Indus::makeShirleyBook1BVHRoot(const Transform4f& renderFromWorld, const Point2f& coatedXZ, const Point2f& glassXZ,[[maybe_unused]] bool enableMotionBlur)
{
  const Transform4f identityTransform{ Mat4f::identity(), Mat4f::identity() };
  std::vector<std::shared_ptr<Primitive>> primitives;
  primitives.reserve(520);

  const CameraShutter shutter{ m_camera->getShutter() };
  const Float shutterOpen{ shutter.shutterOpen };
  const Float shutterClose{ shutter.shutterClose };
  const Float shutterInterval{ shutterClose - shutterOpen };

  auto surfaceCenterY = [](Float worldX, Float worldZ, Float sphereRadius) -> Float
    {
      const Float groundRadius{ 1000 };
      const Float groundCenterY{ -groundRadius };
      const Float inside{ groundRadius * groundRadius - (worldX * worldX + worldZ * worldZ) };
      const double surfaceY{ static_cast<double>(groundCenterY) + std::sqrt(std::max(0.0, static_cast<double>(inside))) };
      return static_cast<Float>(surfaceY) + sphereRadius;
    };

  auto makeSphere = [&](Float sphereRadius, std::shared_ptr<Material> material) -> std::shared_ptr<Primitive>
    {
      auto sphereShape = std::make_shared<Sphere>(identityTransform, identityTransform, false, sphereRadius, -sphereRadius, sphereRadius, Float{ 360 });
      return std::make_shared<GeometricPrimitive>(std::move(sphereShape), std::move(material));
    };

  auto addStaticSphere = [&](const Point3f& worldCenter, Float sphereRadius, std::shared_ptr<Material> material)
    {
      auto basePrimitive = makeSphere(sphereRadius, std::move(material));
      const Transform4f renderFromObject{ renderFromWorld * Transform4f::translate(Vec3f{ worldCenter[0], worldCenter[1], worldCenter[2] }) };
      primitives.push_back(std::make_shared<TransformedPrimitive>(std::move(basePrimitive), renderFromObject));
    };

  auto addFallingSphere = [&](const Point3f& restCenter, Float startCenterY, Float sphereRadius, std::shared_ptr<Material> material)
    {
      auto basePrimitive = makeSphere(sphereRadius, std::move(material));
      const Transform4f startRenderFromObject{ renderFromWorld * Transform4f::translate(Vec3f{ restCenter[0], startCenterY, restCenter[2] }) };
      const Transform4f endRenderFromObject{ renderFromWorld * Transform4f::translate(Vec3f{ restCenter[0], restCenter[1], restCenter[2] }) };
      const AnimatedTransform renderFromObjectAnimation{ startRenderFromObject, shutterOpen, endRenderFromObject, shutterClose };
      primitives.push_back(std::make_shared<AnimatedPrimitive>(std::move(basePrimitive), renderFromObjectAnimation));
    };

  const Float groundRadius{ 1000 };
  addStaticSphere(Point3f{ Float{ 0 }, -groundRadius, Float{ 0 } }, groundRadius, std::make_shared<Diffuse>(ColorRGB{ Float{ 0.5 }, Float{ 0.5 }, Float{ 0.5 } }));

  const Float heroRadius{ 1 };
  const Point3f coatedCenter{ coatedXZ[0], surfaceCenterY(coatedXZ[0], coatedXZ[1], heroRadius), coatedXZ[1] };
  const Point3f glassCenter{ glassXZ[0], surfaceCenterY(glassXZ[0], glassXZ[1], heroRadius), glassXZ[1] };

  addStaticSphere(coatedCenter, heroRadius, std::make_shared<MCoatedDiffuse>(ColorRGB{ Float{ 0.8 }, Float{ 0.2 }, Float{ 0.2 } }, Float{ 1.5 }, Float{ 0.1 }));
  addStaticSphere(glassCenter, heroRadius, std::make_shared<MDielectric>(ColorRGB{ 1, 1, 1 }, ColorRGB{ 1, 1, 1 }, Float{ 1 }, Float{ 1.5 }));

  std::mt19937_64 rng{ 0xD15EA5Eull };
  std::uniform_real_distribution<Float> uniform01{ Float{ 0 }, Float{ 1 } };
  std::uniform_real_distribution<Float> uniformAlbedo{ Float{ 0.15 }, Float{ 0.95 } };

  const Float smallRadiusMin{ Float{ 0.18 } };
  const Float smallRadiusMax{ Float{ 0.32 } };
  const Float noSpawnPadding{ Float{ 0.06 } };

  const Float blurRegionCenterX{ (coatedCenter[0] + glassCenter[0]) * Float { 0.5 } };
  const Float blurRegionCenterZ{ (coatedCenter[2] + glassCenter[2]) * Float { 0.5 } };
  const Float blurRegionSpan{ Float{ 6 } };

  const Float maxMotionDiameters{ Float{ 1.25 } };

  for (int cellZ{ -11 }; cellZ < 11; ++cellZ)
  {
    for (int cellX{ -11 }; cellX < 11; ++cellX)
    {
      Float radius{ 0 };
      Point3f restCenter{ 0, 0, 0 };
      bool placed{ false };

      for (int attempt{}; attempt < 5 && !placed; ++attempt)
      {
        const Float worldX{ static_cast<Float>(cellX) + Float{ 0.9 } * uniform01(rng) };
        const Float worldZ{ static_cast<Float>(cellZ) + Float{ 0.9 } * uniform01(rng) };
        radius = lerp(uniform01(rng), smallRadiusMin, smallRadiusMax);
        restCenter = Point3f{ worldX, surfaceCenterY(worldX, worldZ, radius), worldZ };

        const Float coatedDx{ restCenter[0] - coatedCenter[0] };
        const Float coatedDz{ restCenter[2] - coatedCenter[2] };
        const Float glassDx{ restCenter[0] - glassCenter[0] };
        const Float glassDz{ restCenter[2] - glassCenter[2] };
        const Float coatedClear{ heroRadius + radius + noSpawnPadding };
        const Float glassClear{ heroRadius + radius + noSpawnPadding };

        if (coatedDx * coatedDx + coatedDz * coatedDz < coatedClear * coatedClear) continue;
        if (glassDx * glassDx + glassDz * glassDz < glassClear * glassClear) continue;

        placed = true;
      }

      if (!placed) continue;

      const Float materialSelector{ uniform01(rng) };
      const Float r{ uniformAlbedo(rng) };
      const Float g{ uniformAlbedo(rng) };
      const Float b{ uniformAlbedo(rng) };
      const ColorRGB baseColor{ r * r, g * g, b * b };

      std::shared_ptr<Material> material;
      if (materialSelector < Float{ 0.78 })
      {
        material = std::make_shared<Diffuse>(baseColor);
      }
      else if (materialSelector < Float{ 0.94 })
      {
        const Float coatEta{ lerp(uniform01(rng), Float{ 1.25 }, Float{ 1.6 }) };
        const Float coatRoughness{ lerp(uniform01(rng), Float{ 0.05 }, Float{ 0.35 }) };
        material = std::make_shared<MCoatedDiffuse>(baseColor, coatEta, coatRoughness);
      }
      else
      {
        material = std::make_shared<MDielectric>(ColorRGB{ 1, 1, 1 }, ColorRGB{ 1, 1, 1 }, Float{ 1 }, Float{ 1.5 });
      }

      const Float regionDx{ restCenter[0] - blurRegionCenterX };
      const Float regionDz{ restCenter[2] - blurRegionCenterZ };
      const bool inBlurRegion{ std::abs(regionDx) <= blurRegionSpan && std::abs(regionDz) <= blurRegionSpan };
      const bool shouldAnimate{ enableMotionBlur && inBlurRegion && (shutterInterval > Float{ 0 }) };

      if (!shouldAnimate)
      {
        addStaticSphere(restCenter, radius, std::move(material));
        continue;
      }

      const Float motionDiameters{ maxMotionDiameters * uniform01(rng) };
      const Float deltaY{ motionDiameters * (Float{ 2 } * radius) };
      addFallingSphere(restCenter, restCenter[1] + deltaY, radius, std::move(material));
    }
  }

  return std::make_shared<BVHAggregate>(std::move(primitives), 4, BVHSplitMethod::SAH);
}

void Indus::initializeParallelSystems(std::size_t numThreads) noexcept
{
  if (!m_engineThreadPool)
  {
    m_engineThreadPool = std::make_unique<ThreadPool>(numThreads);
    ParallelSystems::setEngineThreadPool(*m_engineThreadPool);
  }
}

void Indus::shutdownParallelSystems() noexcept
{
  if (m_engineThreadPool)
  {
    m_engineThreadPool->shutdownThreadPool();
    m_engineThreadPool.reset();

    ParallelSystems::resetThreadPool();
  }
}
