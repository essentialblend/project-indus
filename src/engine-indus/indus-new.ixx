export module indus;

import std;
import spherenew;
import colorrgb;
import factory;
import film;
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

export class Indus final
{
public:
  explicit Indus(const IndusConfig& cfg) noexcept;

  void setup();
  void run();

private:
  IndusConfig m_cfg{};

  std::unique_ptr<Film> m_film{};
  std::unique_ptr<CameraBase> m_camera{};
  std::unique_ptr<Sampler> m_sampler{};
  std::unique_ptr<Integrator> m_integrator{};

  std::shared_ptr<Primitive> makeShirleyBook1BVHRoot(const Transform4f& renderFromWorld);
  std::shared_ptr<Primitive> makeLegacyHeroScene(const Transform4f& renderFromWorld);
};

Indus::Indus(const IndusConfig& cfg) noexcept : m_cfg{ cfg } {}

void Indus::setup()
{
  m_film = makeFilm(m_cfg.filmCfg);
  m_camera = makeCamera(m_cfg.camCfg, *m_film);
  m_sampler = makeSampler(m_cfg.samplerCfg);
  m_integrator = makeIntegrator(m_cfg.integratorCfg, *m_camera, *m_sampler);
}

void Indus::run()
{
  setup();

  // get renderFromWorld from the camera transform
  const Transform4f renderFromWorld{
    m_camera->getCameraTransform().getRenderFromWorld()
  };

  // build Shirley scene BVH root (Middle split, 4 prims/node inside)
  std::shared_ptr<Primitive> root{ makeShirleyBook1BVHRoot(renderFromWorld) };

  // wrap in Scene
  Scene scene{ root };

  // render
  RenderTimer timer{};
  m_integrator->render(scene);
  timer.stopTimer();

  // print BVH stats
  if (auto bvh = std::dynamic_pointer_cast<BVHAggregate>(scene.getSceneRoot()))
    bvh->printBVHStats(m_film->getFilmResolution()[0], m_film->getFilmResolution()[1], m_cfg.samplerCfg.samplesPerPixel, timer.getMillisec());

  m_film->writeImage(m_cfg, timer);
}

std::shared_ptr<Primitive>
Indus::makeShirleyBook1BVHRoot(const Transform4f& renderFromWorld)
{
  auto hash01 = [](int a, int b, int k) noexcept -> Float {
    std::uint32_t v = std::uint32_t(a * 73856093) ^ std::uint32_t(b * 19349663) ^ std::uint32_t(k * 83492791);
    v ^= v >> 17; v *= 0xED5AD4BBu; v ^= v >> 11; v *= 0xAC4C1B51u; v ^= v >> 15; v *= 0x31848BABu; v ^= v >> 14;
    return Float(v & 0x00FFFFFFu) / Float(16777216.0f);
    };

  auto makePrim = [&](const Point3f& c, Float r, std::shared_ptr<Material> m) {
    const Transform4f worldFromObject{ Transform4f::translate(Vec3f{ c[0], c[1], c[2] }) };
    const Transform4f renderFromObject{ renderFromWorld * worldFromObject };
    const Transform4f objectFromRender{ Transform4f{ renderFromObject.getInv(), renderFromObject.get() } };
    auto s = std::make_shared<Sphere>(renderFromObject, objectFromRender, false, r, -r, r, Float{ 360 });
    return std::make_shared<GeometricPrimitive>(s, std::move(m));
    };

  std::vector<std::shared_ptr<Primitive>> prims;

  // ground
  prims.push_back(makePrim(Point3f{ 0,-1000,0 }, Float{ 1000 }, std::make_shared<Diffuse>(ColorRGB{ .5f,.5f,.5f })));

  // canonical big spheres: center front, sides pushed slightly back along +z
  const Float Rbig{ 1 }, zBack{ -1.5f };
  const Point3f Cc{ 0,1,0 }, Cl{ -4,1,zBack }, Cr{ 4,1,zBack };
  prims.push_back(makePrim(Cc, Rbig, std::make_shared<MDielectric>(ColorRGB{ 1,1,1 }, ColorRGB{ 1,1,1 }, Float{ 1 }, Float{ 1.5 })));
  prims.push_back(makePrim(Cl, Rbig, std::make_shared<Diffuse>(ColorRGB{ .4f,.2f,.1f })));
  prims.push_back(makePrim(Cr, Rbig, std::make_shared<Diffuse>(ColorRGB{ .7f,.6f,.5f })));

  // small spheres with simple collision evasion (big–small and small–small)
  std::vector<Point3f> accepted; accepted.reserve(600);
  const Float rSmall{ .2f }, pad{ .02f };
  for (int a = -11; a < 11; ++a) for (int b = -11; b < 11; ++b) {
    const Point3f c{ Float(a) + .9f * hash01(a,b,1), rSmall, Float(b) + .9f * hash01(a,b,2) };

    // keep away from the three big spheres
    if (euclideanLength(c - Cc) < (Rbig + rSmall + pad)) continue;
    if (euclideanLength(c - Cl) < (Rbig + rSmall + pad)) continue;
    if (euclideanLength(c - Cr) < (Rbig + rSmall + pad)) continue;

    // keep away from previously placed small spheres
    bool clash{};
    for (const auto& p : accepted)
      if (euclideanLength(c - p) < (Float{ 2 }*rSmall + pad)) { clash = true; break; }
    if (clash) continue;

    accepted.push_back(c);

    // material
    std::shared_ptr<Material> m;
    if (hash01(a, b, 3) < .85f) {
      const ColorRGB c1{ hash01(a,b,4),hash01(a,b,5),hash01(a,b,6) };
      const ColorRGB c2{ hash01(a,b,7),hash01(a,b,8),hash01(a,b,9) };
      m = std::make_shared<Diffuse>(ColorRGB{ c1[0] * c2[0], c1[1] * c2[1], c1[2] * c2[2] });
    }
    else {
      m = std::make_shared<MDielectric>(ColorRGB{ 1,1,1 }, ColorRGB{ 1,1,1 }, Float{ 1 }, Float{ 1.5 });
    }
    prims.push_back(makePrim(c, rSmall, m));
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
      for (const auto& pr : placed) {
        const Float dx = c[0] - pr.first[0], dz = c[2] - pr.first[2];
        if (std::sqrt(dx * dx + dz * dz) < r + pr.second + Float(0.05)) return true;
      }
      return false;
    };

  // ring placement around heroes with corridor exclusion
  for (int i = 0; i < 220; ++i)
  {
    const Float rr = Float(0.10) + Float(0.30) * rf();
    const Float a = -kPi + Float(2) * kPi * rf();
    const Float rad = Float(2.2) + Float(6.0) * std::sqrt(rf());
    const Float x = rad * std::sin(a);
    const Float z = Float(0.8) + rad * std::cos(a);

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