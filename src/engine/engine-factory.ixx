export module indus.engine.factory;

import std;

import indus.core.types;
import indus.core.geom.animatedtransform;
import indus.core.colorspace;

import indus.engine.constructs;
import indus.engine.systems;

import indus.camera.base;
import indus.camera.perspective;
import indus.camera.transform;
import indus.camera.constructs;

import indus.film.base;
import indus.film.rgb;
import indus.film.pixelsensor;
import indus.film.filter;

import indus.sampler.base;
import indus.sampler.independent;
import indus.sampler.stratified;

import indus.integrator.base;
import indus.integrator.path;

import indus.rng.pcg32;

import indus.utilities.basictimer;

import indus.ui.displaysink_base;
import indus.ui.sfml_sink;

export class EngineSystemsFactory final
{
public:
  static EngineSystemsFactory& getInstance() noexcept;

  [[nodiscard]] std::unique_ptr<CameraBase> makeCamera(const CameraConfig& cfg, FilmBase& film) const;

  [[nodiscard]] std::unique_ptr<FilmBase> makeFilm(const FilmConfig& cfg) const;

  [[nodiscard]] std::unique_ptr<Sampler> makeSampler(const SamplerConfig& cfg, Int seed = 0) const;

  [[nodiscard]] std::unique_ptr<Integrator> makeIntegrator(const IntegratorConfig& cfg, CameraBase& camera, Sampler& sampler) const;

  [[nodiscard]] constexpr EngineBuildInformation makeDefaultEngineBuildInformation() noexcept;

  [[nodiscard]] std::unique_ptr<DisplaySinkBase> makeDisplaySink(const ImmutableEngineSystems& immutableEngineSystems, const DisplaySinkConfig& displaySinkCfg, const BasicTimer& basicTimer, const EngineBuildInformation& buildInfo, const FilmConfig& filmCfg);

private:
  EngineSystemsFactory() = default;
};

EngineSystemsFactory& EngineSystemsFactory::getInstance() noexcept
{
  static EngineSystemsFactory inst{};
  return inst;
}

std::unique_ptr<CameraBase> EngineSystemsFactory::makeCamera(const CameraConfig& cfg, FilmBase& film) const
{
  const Float startTime{ cfg.cameraShutter.shutterOpen };
  const Float endTime{ cfg.cameraShutter.shutterClose };

  const AnimatedTransform worldFromCamera{ cfg.cameraToWorld, startTime, cfg.cameraToWorld, endTime };

  const CameraTransform cameraTransform{ worldFromCamera, cfg.renderingSpace };

  return std::make_unique<PerspectiveCamera>(cameraTransform, cfg.cameraToWorld, cfg.cameraShutter, film, cfg.fovDegrees, cfg.screenWindow, cfg.lensRadius, cfg.focalDistance);
}

std::unique_ptr<FilmBase> EngineSystemsFactory::makeFilm(const FilmConfig& cfg) const
{
  const RGBColorSpace& colorSpace{ ColorRegistry::sRGB() };

  std::unique_ptr<Filter> filter{ std::make_unique<BoxFilter>(cfg.filterRadius) };

  const PixelSensor sensor{ colorSpace.XYZFromRGB, colorSpace, cfg.imagingRatio };

  return std::make_unique<RGBFilm>(cfg.resolution, cfg.crop, cfg.diagonalMM,
    std::move(filter), sensor);
}

std::unique_ptr<Sampler> EngineSystemsFactory::makeSampler(const SamplerConfig& cfg, Int seed) const
{
  auto rng{ std::make_unique<PCG32>() };

  if (cfg.isStratified)
  {
    return std::make_unique<StratifiedSampler>(cfg.strata, cfg.isJitter, static_cast<Int64>(seed), std::move(rng));
  }

  const Int spp{ std::max<Int>(1, cfg.samplesPerPixel) };

  return std::make_unique<IndependentSampler>(spp, static_cast<UInt64>(seed), std::move(rng));
}

std::unique_ptr<Integrator> EngineSystemsFactory::makeIntegrator(const IntegratorConfig& cfg, CameraBase& camera, Sampler& sampler) const
{
  return std::make_unique<PathIntegrator>(camera, sampler, cfg.maxDepth, cfg.useRR);
}

constexpr EngineBuildInformation EngineSystemsFactory::makeDefaultEngineBuildInformation() noexcept
{
  return EngineBuildInformation
  {
    "Project Indus",
    INDUS_VERSION_STRING,
    INDUS_BUILD_CONFIG,
    INDUS_GIT_HASH,
    INDUS_BUILD_TIMESTAMP,
    INDUS_ENGINE_AUTHOR
  };
}

std::unique_ptr<DisplaySinkBase> EngineSystemsFactory::makeDisplaySink(const ImmutableEngineSystems& immutableEngineSystems, const DisplaySinkConfig& displaySinkCfg, const BasicTimer& basicTimer, const EngineBuildInformation& buildInfo, const FilmConfig& filmCfg)
{
  if (displaySinkCfg.sinkType == SinkType::SFML)
  {
    return std::make_unique<SFMLDisplaySink>(filmCfg.resolution, basicTimer, buildInfo, immutableEngineSystems, displaySinkCfg);
  }

  throw std::runtime_error("Unknown sink type in EngineSystemsFactory");
}
