export module indus.engine;

import std;

import indus.core.math.constants.i;
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

  std::shared_ptr<Primitive> makeShirleyBook1BVHRoot(const Transform4f& renderFromWorld, const Point2f& coatedXZ, const Point2f& glassXZ = {});

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

  std::shared_ptr<Primitive> sceneRootAggregate{ makeShirleyBook1BVHRoot(m_camera->getCameraTransform().getRenderFromWorld(), matteBallPosition, dielectricBallPosition) };

  m_renderScene = std::make_unique<Scene>(sceneRootAggregate);

  m_engineBuildInfo = engineSystemsFactory.makeDefaultEngineBuildInformation();
  m_engineBuildInfo.runtimeThreads = hardwareThreads != 0u ? hardwareThreads : 1u;

  ImmutableEngineSystems immutables{ *m_film, *m_camera, *m_sampler, *m_integrator, *m_renderScene };

  m_displaySink = engineSystemsFactory.makeDisplaySink(immutables, m_cfg.displaySinkCfg, m_HUDTimer, m_engineBuildInfo, m_cfg.filmCfg);

  StatsAccumulator::setFilmBytes(static_cast<std::uint64_t>(m_film->getFilmResolution()[0]) * (m_film->getFilmResolution()[1]) * sizeof(Pixel));
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
  std::uint64_t lastVersion{ 0 };

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

//std::shared_ptr<Primitive> Indus::makeShirleyBook1BVHRoot(const Transform4f& renderFromWorld, const Point2f& matteXZ, const Point2f& glassXZ)
//{
//  std::vector<std::shared_ptr<Primitive>> prims;
//
//  const Point3f gc{ 0, -1000, 0 }; const Float gRGround{ 1000 };
//  {
//    const Transform4f wO{ Transform4f::translate(Vec3f{ gc[0], gc[1], gc[2] }) };
//    const Transform4f rO{ renderFromWorld * wO };
//    const Transform4f oR{ Transform4f{ rO.getInv(), rO.get() } };
//    auto s = std::make_shared<Sphere>(rO, oR, false, gRGround, -gRGround, gRGround, Float{ 360 });
//    auto m = std::make_shared<Diffuse>(ColorRGB{ Float{0.5}, Float{0.5}, Float{0.5} });
//    prims.push_back(std::make_shared<GeometricPrimitive>(s, m));
//  }
//
//  auto surfaceY = [](Float x, Float z, Float r)->Float {
//    const Float R{ 1000 }, cy{ -1000 }; const Float t{ R * R - (x * x + z * z) };
//    const double yg{ static_cast<double>(cy) + std::sqrt(std::max(0.0, static_cast<double>(t))) };
//    return static_cast<Float>(yg) + r;
//    };
//
//  const Float gR{ 1.0f }, gIOR{ 1.5f };
//  const Point3f gC{ glassXZ[0], surfaceY(glassXZ[0], glassXZ[1], gR), glassXZ[1] };
//  {
//    const Transform4f wO{ Transform4f::translate(Vec3f{ gC[0], gC[1], gC[2] }) };
//    const Transform4f rO{ renderFromWorld * wO };
//    const Transform4f oR{ Transform4f{ rO.getInv(), rO.get() } };
//    auto s = std::make_shared<Sphere>(rO, oR, false, gR, -gR, gR, Float{ 360 });
//    auto m = std::make_shared<MDielectric>(ColorRGB{ 1, 1, 1 }, ColorRGB{ 1, 1, 1 }, Float{ 1 }, gIOR);
//    prims.push_back(std::make_shared<GeometricPrimitive>(s, m));
//  }
//
//  const Float mR{ 1.0f };
//  const Point3f mC{ matteXZ[0], surfaceY(matteXZ[0], matteXZ[1], mR), matteXZ[1] };
//  {
//    const Transform4f wO{ Transform4f::translate(Vec3f{ mC[0], mC[1], mC[2] }) };
//    const Transform4f rO{ renderFromWorld * wO };
//    const Transform4f oR{ Transform4f{ rO.getInv(), rO.get() } };
//    auto s{ std::make_shared<Sphere>(rO, oR, false, mR, -mR, mR, Float{ 360 }) };
//    auto m{ std::make_shared<MCoatedDiffuse>(ColorRGB{ Float{ 0.8 }, Float{ 0.2 }, Float{ 0.2 } }, Float{ 1.5 }, Float{ 0.5 }) };
//    prims.push_back(std::make_shared<GeometricPrimitive>(s, m));
//  }
//
//  const int N{ 650 }; const Float rmin{ 0.175f }, rmax{ 0.33f }, pad{ 0.025f };
//  const Float xmin{ -15 }, xmax{ 15 }, zmin{ -10 }, zmax{ 15 };
//  std::mt19937_64 rng{ 0xC0FFEEull };
//  std::uniform_real_distribution<Float> ux(xmin, xmax), uz(zmin, zmax), ur(rmin, rmax), u01(0, 1), uc(0.2f, 0.9f);
//
//  std::vector<Point3f> centers; centers.reserve(N + 2);
//  std::vector<Float>   radii;   radii.reserve(N + 2);
//  centers.push_back(gC); radii.push_back(gR);
//  centers.push_back(mC); radii.push_back(mR);
//
//  int attempts{}; const int maxAttempts{ 10000 };
//  while (static_cast<int>(centers.size()) - 2 < N && attempts++ < maxAttempts)
//  {
//    const Float x{ ux(rng) }, z{ uz(rng) }, r{ ur(rng) };
//    const Point3f c{ x, surfaceY(x, z, r), z };
//
//    bool clash{};
//    for (size_t i{}; i < centers.size(); ++i) 
//    {
//      const Float dist2{ euclideanLengthSq(centers[i] - c) };
//      const Float rr{ radii[i] + r + pad };
//      if (dist2 < rr * rr) { clash = true; break; }
//    }
//    
//    if (clash) continue;
//
//    centers.push_back(c); radii.push_back(r);
//
//    std::shared_ptr<Material> mat;
//    if (u01(rng) < Float{ 0.75 }) 
//    {
//      const ColorRGB a{ uc(rng), uc(rng), uc(rng) }, b{ uc(rng), uc(rng), uc(rng) };
//      mat = std::make_shared<Diffuse>(ColorRGB{ a[0] * b[0], a[1] * b[1], a[2] * b[2] });
//    }
//    else 
//    {
//      mat = std::make_shared<MDielectric>(ColorRGB{ 1, 1, 1 }, ColorRGB{ 1, 1, 1 }, Float{ 1 }, Float{ 1.5 });
//    }
//
//    const Transform4f wO{ Transform4f::translate(Vec3f{ c[0], c[1], c[2] }) };
//    const Transform4f rO{ renderFromWorld * wO };
//    const Transform4f oR{ Transform4f{ rO.getInv(), rO.get() } };
//    auto s = std::make_shared<Sphere>(rO, oR, false, r, -r, r, Float{ 360 });
//    prims.push_back(std::make_shared<GeometricPrimitive>(s, mat));
//  }
//
//  // Ad-hoc, needs principled replacement later? More than just spheres as a Primitive type? CAUTION.
//  StatsAccumulator::addGeometryBytes(std::uint64_t(prims.size()) * sizeof(GeometricPrimitive));
//
//  return std::make_shared<BVHAggregate>(std::move(prims), 4, BVHSplitMethod::SAH);
//}

std::shared_ptr<Primitive> Indus::makeShirleyBook1BVHRoot(const Transform4f& renderFromWorld, const Point2f& coatedXZ, const Point2f& glassXZ)
{
  std::vector<std::shared_ptr<Primitive>> prims;
  prims.reserve(704);

  auto addSphere = [&](const Point3f& center, Float radius, const std::shared_ptr<Material>& material)
  {
    const Transform4f worldFromObject{ Transform4f::translate(Vec3f{ center[0], center[1], center[2] }) };
    const Transform4f renderFromObject{ renderFromWorld * worldFromObject };
    const Transform4f objectFromRender{ Transform4f{ renderFromObject.getInv(), renderFromObject.get() } };

    auto sphere{ std::make_shared<Sphere>(renderFromObject, objectFromRender, false, radius, -radius, radius, Float{ 360 }) };

    prims.push_back(std::make_shared<GeometricPrimitive>(sphere, material));
  };

  const Float groundR{ 1000 };
  const Point3f groundC{ Float{0}, Float{-1000}, Float{0} };
  addSphere(groundC, groundR, std::make_shared<Diffuse>(ColorRGB{ Float{0.5}, Float{0.5}, Float{0.5} }));

  const auto surfaceY = [](Float x, Float z, Float r) noexcept -> Float
  {
    const Float R{ 1000 };
    const Float cy{ -1000 };
    const Float t{ R * R - (x * x + z * z) };
    const double yGeom{ static_cast<double>(cy) + std::sqrt(std::max(0.0, static_cast<double>(t))) };
    
    return static_cast<Float>(yGeom) + r;
  };

  const Float heroR{ 1.0f };

  const Point3f coatedC{ coatedXZ[0], surfaceY(coatedXZ[0], coatedXZ[1], heroR), coatedXZ[1] };
  
  addSphere(coatedC, heroR, std::make_shared<MCoatedDiffuse>(ColorRGB{ Float{0.8}, Float{0.2}, Float{0.2} }, Float{ 1.5f }, Float{ 0.1f }));

  const Point3f glassC{ glassXZ[0], surfaceY(glassXZ[0], glassXZ[1], heroR), glassXZ[1] };
  addSphere(glassC, heroR, std::make_shared<MDielectric>(ColorRGB{ 1, 1, 1 }, ColorRGB{ 1, 1, 1 }, Float{ 1 }, Float{ 1.5 }));

  const int N{ 650 };
  const Float rmin{ 0.175f }, rmax{ 0.33f }, pad{ 0.025f };
  const Float xmin{ -15 }, xmax{ 15 }, zmin{ -10 }, zmax{ 15 };

  std::mt19937_64 rng{ 0xC0FFEEull };
  std::uniform_real_distribution<Float> ux(xmin, xmax);
  std::uniform_real_distribution<Float> uz(zmin, zmax);
  std::uniform_real_distribution<Float> ur(rmin, rmax);
  std::uniform_real_distribution<Float> u01(Float{ 0 }, Float{ 1 });
  std::uniform_real_distribution<Float> uc(Float{ 0.2f }, Float{ 0.9f });

  std::vector<Point3f> centers;
  std::vector<Float> radii;

  centers.reserve(N + 2);
  radii.reserve(N + 2);

  centers.push_back(coatedC); radii.push_back(heroR);
  centers.push_back(glassC);  radii.push_back(heroR);

  int attempts{};
  const int maxAttempts{ 10000 };

  while (static_cast<int>(centers.size()) - 2 < N && attempts++ < maxAttempts)
  {
    const Float x{ ux(rng) };
    const Float z{ uz(rng) };
    const Float r{ ur(rng) };

    const Point3f c{ x, surfaceY(x, z, r), z };

    bool clash{};

    for (std::size_t i{}; i < centers.size(); ++i)
    {
      const Float dist2{ euclideanLengthSq(centers[i] - c) };
      const Float rr{ radii[i] + r + pad };
      if (dist2 < rr * rr) { clash = true; break; }
    }
    if (clash) continue;

    centers.push_back(c);
    radii.push_back(r);

    const ColorRGB a{ uc(rng), uc(rng), uc(rng) };
    const ColorRGB b{ uc(rng), uc(rng), uc(rng) };
    const ColorRGB baseReflectance{ a[0] * b[0], a[1] * b[1], a[2] * b[2] };

    const Float uMat{ u01(rng) };
    Float coatEta{};
    Float coatRoughness{};

    if (uMat < Float{ 0.3f })
    {
      coatEta = Float{ 1.0f };
      coatRoughness = Float{ 0.5f } + Float{ 0.4f } * u01(rng);
    }
    else
    {
      coatEta = Float{ 1.3f } + Float{ 0.4f } * u01(rng);
      coatRoughness = Float{ 0.02f } + Float{ 0.4f } * u01(rng);
    }

    auto mat{ std::make_shared<MCoatedDiffuse>(baseReflectance, coatEta, coatRoughness) };

    addSphere(c, r, mat);
  }

  StatsAccumulator::addGeometryBytes(std::uint64_t(prims.size()) * sizeof(GeometricPrimitive));

  return std::make_shared<BVHAggregate>(std::move(prims), 4, BVHSplitMethod::SAH);
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
