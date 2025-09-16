export module indus;

import std;
import sphere;
import colorrgb;
import factory;
import film;
import camerabase;
import sampler;
import integrator;
import constructs;
import glass;
import matte;
import world_object;

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

  auto glassMat = std::make_shared<MGlass>(ColorRGB{ 1 }, ColorRGB{ 1 }, Float(1.0), Float(1.4));
  auto glassBall = std::make_unique<WOSphere>(Point3f(Float(0), Float(1), Float(0)), Float(1.1), glassMat);

  auto ballMat1 = std::make_shared<MMatte>(ColorRGB(Float(0.8), Float(0.2), Float(0.6)));
  auto ball1 = std::make_unique<WOSphere>(Point3f(Float(- 1.5), Float(0.66), Float(- 1.5)), Float(0.65), ballMat1);

  auto ballMat2 = std::make_shared<MMatte>(ColorRGB(Float(0.2), Float(0.8), Float(0.2)));
  auto ball2 = std::make_unique<WOSphere>(Point3f(Float(0.7), Float(0.66), Float(+ 5)), Float(0.65), ballMat2);

  auto groundMat = std::make_shared<MMatte>(ColorRGB(Float(0.45), Float(0.45), Float(0.45)));
  auto groundBall = std::make_unique<WOSphere>(Point3f(Float(0), Float(- 1000), Float(0)), Float(1000), groundMat);

  world->addWorldObj(std::move(groundBall));
  //world->addWorldObj(std::move(glassBall));
  //world->addWorldObj(std::move(ball1));
  //world->addWorldObj(std::move(ball2));

  m_integrator->render(*world);

  m_film->writeImage("out/debug5.png");
}