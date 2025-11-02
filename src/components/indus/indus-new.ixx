module; 
#include <SFML/Graphics.hpp>

export module indus;

import std;
import spherenew;
import colorrgb;
import factory;
import filmbase;
import camerabase;
import sampler;
import integrator;
import engineconstructs;
import dielectric;
import diffuse;
import listaggregate;
import pcg32;
import material;
import primitive;
import scene;
import miscconstructs;
import bvhaggregate;
import mathalgebra;
import mathconstants;
import mathfp;
import threadpool;
import parallel;
import sfmlsink;
import dirtylatch;

export class Indus final
{
public:
  explicit Indus(const IndusConfig& cfg) noexcept;

  void setupEngineSystems();
  void run();

private:
  IndusConfig m_cfg{};

  std::unique_ptr<FilmBase> m_film{};
  std::unique_ptr<CameraBase> m_camera{};
  std::unique_ptr<Sampler> m_sampler{};
  std::unique_ptr<Integrator> m_integrator{};
  std::unique_ptr<ThreadPool> m_engineThreadPool{};

  DirtyLatch m_dirtyLatch{};
  RuntimeSharedState m_runtimeSharedState{};
  RuntimeComponents m_runtimeComponents{};
  
  std::shared_ptr<Primitive> makeShirleyBook1BVHRoot(const Transform4f& renderFromWorld, const Point2f& matteXZ, const Point2f& glassXZ = {});

  std::shared_ptr<Primitive> makeLegacyHeroScene(const Transform4f& renderFromWorld);

  std::mutex m_displayMutex{};
  std::vector<std::uint8_t> m_displayBytesArr{};

  void initializeParallelSystems(std::size_t numThreads) noexcept;
  void shutdownParallelSystems() noexcept;

  void displaySinkWindow();

  void updateAndPresent(std::vector<uint8_t, std::allocator<sf::Uint8>>& localBytes);
};

Indus::Indus(const IndusConfig& cfg) noexcept : m_cfg{ cfg } {}

void Indus::setupEngineSystems()
{
  m_film = makeFilm(m_cfg.filmCfg);
  
  m_runtimeComponents.displaySinkPtr = std::make_shared<SFMLDisplaySink>(m_film->getFilmResolution());
  m_runtimeComponents.displayMutex = m_displayMutex;
  m_runtimeComponents.displayBytesArr = m_displayBytesArr;
  m_runtimeComponents.dirtyLatch = m_dirtyLatch;
  m_runtimeComponents.runtimeSharedState = m_runtimeSharedState;

  m_camera = makeCamera(m_cfg.camCfg, *m_film);
  m_sampler = makeSampler(m_cfg.samplerCfg);

  m_integrator = makeIntegrator(m_runtimeComponents, m_cfg.integratorCfg, *m_camera, *m_sampler);
}

void Indus::run()
{
  setupEngineSystems();
  
  initializeParallelSystems(std::max(1u, std::thread::hardware_concurrency()));

  const Transform4f renderFromWorld{
    m_camera->getCameraTransform().getRenderFromWorld()
  };

  const Point2f matteBallPosition{ 0.75, -1.25 };
  const Point2f dielectricBallPosition{ 0, 0 };

  std::shared_ptr<Primitive> root{ makeShirleyBook1BVHRoot(renderFromWorld, matteBallPosition, dielectricBallPosition) };

  Scene scene{ root };

  std::jthread renderThread
  {
    [this, &scene]
    {
      RenderTimer timer{};

      m_integrator->render(scene);

      timer.stopTimer();

      m_film->writeImage(m_cfg, timer);
    }
  };

  displaySinkWindow();
}

void Indus::displaySinkWindow()
{
  std::vector<std::uint8_t> localBytes(static_cast<std::size_t>(m_film->getFilmResolution()[0]) * m_film->getFilmResolution()[1] * 4u, 255);

  while (m_runtimeComponents.displaySinkPtr->isSinkOpen())
  {
    updateAndPresent(localBytes);
  }
}

void Indus::updateAndPresent(std::vector<uint8_t, std::allocator<sf::Uint8>>& localBytes)
{
  const bool doUpload{ m_dirtyLatch.consume() };
  
  {
    std::lock_guard<std::mutex> displayLock(m_displayMutex);

    if (!m_displayBytesArr.empty()) localBytes = m_displayBytesArr;
  }

  const float progress = (m_runtimeComponents.runtimeSharedState) ? m_runtimeComponents.runtimeSharedState->get().progressUnitNormalized.load() : 0.0f;

  DisplayFrame frame{ m_film->getFilmResolution(), std::span<const std::uint8_t>{ localBytes }, doUpload, progress };

  m_runtimeComponents.displaySinkPtr->present(frame);
}

std::shared_ptr<Primitive> Indus::makeShirleyBook1BVHRoot(const Transform4f& renderFromWorld, const Point2f& matteXZ, const Point2f& glassXZ)
{
  std::vector<std::shared_ptr<Primitive>> prims;

  const Point3f gc{ 0, -1000, 0 }; const Float gRGround{ 1000 };
  {
    const Transform4f wO{ Transform4f::translate(Vec3f{ gc[0], gc[1], gc[2] }) };
    const Transform4f rO{ renderFromWorld * wO };
    const Transform4f oR{ Transform4f{ rO.getInv(), rO.get() } };
    auto s = std::make_shared<Sphere>(rO, oR, false, gRGround, -gRGround, gRGround, Float{ 360 });
    auto m = std::make_shared<Diffuse>(ColorRGB{ Float{0.5}, Float{0.5}, Float{0.5} });
    prims.push_back(std::make_shared<GeometricPrimitive>(s, m));
  }

  auto surfaceY = [](Float x, Float z, Float r)->Float {
    const Float R{ 1000 }, cy{ -1000 }; const Float t{ R * R - (x * x + z * z) };
    const double yg{ static_cast<double>(cy) + std::sqrt(std::max(0.0, static_cast<double>(t))) };
    return static_cast<Float>(yg) + r;
    };

  const Float gR{ 1.0f }, gIOR{ 1.5f };
  const Point3f gC{ glassXZ[0], surfaceY(glassXZ[0], glassXZ[1], gR), glassXZ[1] };
  {
    const Transform4f wO{ Transform4f::translate(Vec3f{ gC[0], gC[1], gC[2] }) };
    const Transform4f rO{ renderFromWorld * wO };
    const Transform4f oR{ Transform4f{ rO.getInv(), rO.get() } };
    auto s = std::make_shared<Sphere>(rO, oR, false, gR, -gR, gR, Float{ 360 });
    auto m = std::make_shared<MDielectric>(ColorRGB{ 1, 1, 1 }, ColorRGB{ 1, 1, 1 }, Float{ 1 }, gIOR);
    prims.push_back(std::make_shared<GeometricPrimitive>(s, m));
  }

  const Float mR{ 1.0f };
  const Point3f mC{ matteXZ[0], surfaceY(matteXZ[0], matteXZ[1], mR), matteXZ[1] };
  {
    const Transform4f wO{ Transform4f::translate(Vec3f{ mC[0], mC[1], mC[2] }) };
    const Transform4f rO{ renderFromWorld * wO };
    const Transform4f oR{ Transform4f{ rO.getInv(), rO.get() } };
    auto s = std::make_shared<Sphere>(rO, oR, false, mR, -mR, mR, Float{ 360 });
    auto m = std::make_shared<Diffuse>(ColorRGB{ Float{0.8}, Float{0.2}, Float{0.2} });
    prims.push_back(std::make_shared<GeometricPrimitive>(s, m));
  }

  const int N{ 650 }; const Float rmin{ 0.175f }, rmax{ 0.33f }, pad{ 0.025f };
  const Float xmin{ -15 }, xmax{ 15 }, zmin{ -10 }, zmax{ 15 };
  std::mt19937_64 rng{ 0xC0FFEEull };
  std::uniform_real_distribution<Float> ux(xmin, xmax), uz(zmin, zmax), ur(rmin, rmax), u01(0, 1), uc(0.2f, 0.9f);

  std::vector<Point3f> centers; centers.reserve(N + 2);
  std::vector<Float>   radii;   radii.reserve(N + 2);
  centers.push_back(gC); radii.push_back(gR);
  centers.push_back(mC); radii.push_back(mR);

  int attempts{}; const int maxAttempts{ 10000 };
  while (static_cast<int>(centers.size()) - 2 < N && attempts++ < maxAttempts)
  {
    const Float x{ ux(rng) }, z{ uz(rng) }, r{ ur(rng) };
    const Point3f c{ x, surfaceY(x, z, r), z };

    bool clash{};
    for (size_t i{}; i < centers.size(); ++i) 
    {
      const Float dist2{ euclideanLengthSq(centers[i] - c) };
      const Float rr{ radii[i] + r + pad };
      if (dist2 < rr * rr) { clash = true; break; }
    }
    
    if (clash) continue;

    centers.push_back(c); radii.push_back(r);

    std::shared_ptr<Material> mat;
    if (u01(rng) < Float{ 0.75 }) 
    {
      const ColorRGB a{ uc(rng), uc(rng), uc(rng) }, b{ uc(rng), uc(rng), uc(rng) };
      mat = std::make_shared<Diffuse>(ColorRGB{ a[0] * b[0], a[1] * b[1], a[2] * b[2] });
    }
    else 
    {
      mat = std::make_shared<MDielectric>(ColorRGB{ 1, 1, 1 }, ColorRGB{ 1, 1, 1 }, Float{ 1 }, Float{ 1.5 });
    }

    const Transform4f wO{ Transform4f::translate(Vec3f{ c[0], c[1], c[2] }) };
    const Transform4f rO{ renderFromWorld * wO };
    const Transform4f oR{ Transform4f{ rO.getInv(), rO.get() } };
    auto s = std::make_shared<Sphere>(rO, oR, false, r, -r, r, Float{ 360 });
    prims.push_back(std::make_shared<GeometricPrimitive>(s, mat));
  }

  return std::make_shared<BVHAggregate>(std::move(prims), 4, BVHSplitMethod::SAH);
}

std::shared_ptr<Primitive>
Indus::makeLegacyHeroScene(const Transform4f& renderFromWorld)
{
  auto makePrim = [&](const Point3f& c, Float r, std::shared_ptr<Material> m)
    {
      const Transform4f worldFromObject = Transform4f::translate(Vec3f{ c[0], c[1], c[2] });
      const Transform4f renderFromObject = renderFromWorld * worldFromObject;
      const Transform4f objectFromRender{ Transform4f{ renderFromObject.getInv(), renderFromObject.get() } };
      auto s = std::make_shared<Sphere>(renderFromObject, objectFromRender, false, r, -r, r, Float{ 360 });
      return std::make_shared<GeometricPrimitive>(s, std::move(m));
    };

  std::vector<std::shared_ptr<Primitive>> prims;

  // deterministic RNG matching the original scene
  PCG32 rng; rng.setSeedAndStream(0x9E3779B97F4A7C15ull, 0xC2B2AE3D27D4EB4Full);
  auto rf = [&]() { return rng.uniform<Float>(); };

  // ground
  prims.push_back(makePrim(Point3f{ Float(0), Float(-1000), Float(0) },
    Float(1000),
    std::make_shared<Diffuse>(ColorRGB(Float(0.40), Float(0.42), Float(0.46)))));

  // heroes (glass center, two diffuse sides)
  auto heroGlass = std::make_shared<MDielectric>(ColorRGB{ 1 }, ColorRGB{ 1 }, Float(1.0), Float(1.5));
  auto heroL = std::make_shared<Diffuse>(ColorRGB(Float(0.90), Float(0.35), Float(0.55)));
  auto heroR = std::make_shared<Diffuse>(ColorRGB(Float(0.25), Float(0.70), Float(0.95)));

  std::vector<std::pair<Point3f, Float>> placed;
  prims.push_back(makePrim(Point3f{ Float(0.0),  Float(1.15), Float(0.8) }, Float(1.15), heroGlass)); placed.emplace_back(Point3f{ 0.0f,1.15f,0.8f }, 1.15f);
  prims.push_back(makePrim(Point3f{ Float(-2.4), Float(1.00), Float(1.4) }, Float(1.00), heroL));     placed.emplace_back(Point3f{ -2.4f,1.0f,1.4f }, 1.0f);
  prims.push_back(makePrim(Point3f{ Float(2.4),  Float(0.90), Float(1.8) }, Float(0.90), heroR));     placed.emplace_back(Point3f{ 2.4f,0.9f,1.8f }, 0.9f);

  // palette matching original
  std::array<ColorRGB, 9> palette{
    ColorRGB(Float(0.92), Float(0.35), Float(0.55)),
    ColorRGB(Float(0.25), Float(0.70), Float(0.95)),
    ColorRGB(Float(0.20), Float(0.85), Float(0.35)),
    ColorRGB(Float(0.95), Float(0.75), Float(0.25)),
    ColorRGB(Float(0.70), Float(0.45), Float(0.90)),
    ColorRGB(Float(0.95), Float(0.40), Float(0.20)),
    ColorRGB(Float(0.20), Float(0.90), Float(0.80)),
    ColorRGB(Float(0.85), Float(0.85), Float(0.85)),
    ColorRGB(Float(0.55), Float(0.60), Float(0.70))
  };

  // collision check in x–z plane
  auto collide = [&](const Point3f& c, Float r)
    {
      for (const auto& pr : placed) 
      {
        const Float dx{ c[0] - pr.first[0] }, dz{ c[2] - pr.first[2] };
        if (std::sqrt(dx * dx + dz * dz) < r + pr.second + Float{ 0.05 }) return true;
      }
      return false;
    };

  // ring placement around heroes with corridor exclusion
  for (int i{}; i < 220; ++i)
  {
    const Float rr{ Float(0.10) + Float(0.30) * rf() };
    const Float a{ -kPi + Float(2) * kPi * rf() };
    const Float rad{ Float(2.2) + Float(6.0) * std::sqrt(rf()) };
    const Float x{ rad * std::sin(a) };
    const Float z{ Float(0.8) + rad * std::cos(a) };

    if (std::abs(x) < Float(0.7) && z > Float(0.2) && z < Float(2.0)) continue;

    const Point3f c{ x, rr, z };
    if (collide(c, rr)) continue;

    placed.emplace_back(c, rr);

    if (rf() < Float(0.72))
    {
      const std::size_t idx = std::min<std::size_t>(palette.size() - 1,
        static_cast<std::size_t>(rf() * palette.size()));
      prims.push_back(makePrim(c, rr, std::make_shared<Diffuse>(palette[idx])));
    }
    else
    {
      const Float eta = Float(1.25) + Float(0.5) * rf();
      prims.push_back(makePrim(c, rr, std::make_shared<MDielectric>(ColorRGB{ 1 }, ColorRGB{ 1 }, Float(1.0), eta)));
    }
  }

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
