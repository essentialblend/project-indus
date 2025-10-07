export module indus;

import std;
//import sphere;
import colorrgb;
import factory;
import film;
import camerabase;
import sampler;
import integrator;
import constructs;
import dielectric;
import diffuse;
import world_object;
import pcg32;
import material;
import sphere;

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

  auto world = std::make_unique<WorldObjectList>();

  auto ground = std::make_shared<Diffuse>(ColorRGB(Float(0.40), Float(0.42), Float(0.46)));
  world->addWorldObj(std::make_unique<WOSphere>(Point3f(Float{}, Float(-1000), Float{}), Float(1000), ground));

  PCG32 rng; rng.setSeedAndStream(0x9E3779B97F4A7C15ull, 0xC2B2AE3D27D4EB4Full);
  auto rf = [&]() {return rng.uniform<float>(); };

  std::vector<std::pair<Point3f, Float>> placed;

  auto heroGlass = std::make_shared<MDielectric>(ColorRGB{ 1 }, ColorRGB{ 1 }, Float(1.0), Float(1.5));
  auto heroL = std::make_shared<Diffuse>(ColorRGB(Float(0.90), Float(0.35), Float(0.55)));
  auto heroR = std::make_shared<Diffuse>(ColorRGB(Float(0.25), Float(0.70), Float(0.95)));

  std::vector<std::tuple<Point3f, Float, std::shared_ptr<Material>>> heroes = {
    {Point3f(Float(0.0),Float(1.15),Float(0.8)),Float(1.15),heroGlass},
    {Point3f(Float(-2.4),Float(1.00),Float(1.4)),Float(1.00),heroL},
    {Point3f(Float(2.4),Float(0.90),Float(1.8)),Float(0.90),heroR}
  };
  for (auto& h : heroes) { auto [c, r, m] = h; world->addWorldObj(std::make_unique<WOSphere>(c, r, m)); placed.emplace_back(c, r); }

  std::array<ColorRGB, 9> palette = {
    ColorRGB(Float(0.92),Float(0.35),Float(0.55)),
    ColorRGB(Float(0.25),Float(0.70),Float(0.95)),
    ColorRGB(Float(0.20),Float(0.85),Float(0.35)),
    ColorRGB(Float(0.95),Float(0.75),Float(0.25)),
    ColorRGB(Float(0.70),Float(0.45),Float(0.90)),
    ColorRGB(Float(0.95),Float(0.40),Float(0.20)),
    ColorRGB(Float(0.20),Float(0.90),Float(0.80)),
    ColorRGB(Float(0.85),Float(0.85),Float(0.85)),
    ColorRGB(Float(0.55),Float(0.60),Float(0.70))
  };

  auto collide = [&](const Point3f& c, Float r) {
    for (auto& pr : placed) {
      Float dx = c[0] - pr.first[0], dz = c[2] - pr.first[2];
      if (std::sqrt(dx * dx + dz * dz) < r + pr.second + Float(0.05))return true;
    }
    return false;
    };

  for (int i = 0; i < 220; ++i) {
    Float rr = Float(0.10) + Float(0.30) * rf();
    Float a = Float(-std::numbers::pi_v<float>) + Float(2) * std::numbers::pi_v<float>*rf();
    Float rad = Float(2.2) + Float(6.0) * std::sqrt(rf());
    Float x = rad * std::sin(a), z = Float(0.8) + rad * std::cos(a);
    if (std::abs(x) < Float(0.7) && z > Float(0.2) && z < Float(2.0)) continue;
    Point3f c{ x, rr, z };
    if (collide(c, rr)) continue;
    placed.emplace_back(c, rr);
    if (rf() < 0.72f) {
      auto col = palette[std::min<std::size_t>(palette.size() - 1, std::size_t(rf() * palette.size()))];
      world->addWorldObj(std::make_unique<WOSphere>(c, rr, std::make_shared<Diffuse>(col)));
    }
    else {
      Float eta = Float(1.25) + Float(0.5) * rf();
      world->addWorldObj(std::make_unique<WOSphere>(c, rr, std::make_shared<MDielectric>(ColorRGB{ 1 }, ColorRGB{ 1 }, Float(1.0), eta)));
    }
  }
  m_integrator->render(*world);

  m_film->writeImage(m_cfg);
}