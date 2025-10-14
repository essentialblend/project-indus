export module factory;

import std;
import constructs;
import types;
import camerabase;
import film;
import perspectivecamera;
import sampler;
import independentsampler;
import stratifiedsampler;
import integrator;
import pathintegrator;
import pcg32;
import lcg;
import animatedtransform;
import cameratransform;

export std::unique_ptr<CameraBase> makeCamera(const CameraConfig& cfg, Film& film)
{
  const Float startTime{ cfg.cameraShutter.shutterOpen };
  const Float endTime{ cfg.cameraShutter.shutterClose };

  const AnimatedTransform worldFromCamera{ cfg.cameraToWorld, startTime, cfg.cameraToWorld, endTime };

  const CameraTransform cameraTransform{ worldFromCamera, cfg.renderingSpace };

  return std::make_unique<PerspectiveCamera>(cameraTransform, cfg.cameraToWorld, cfg.cameraShutter, film, cfg.fovDegrees, cfg.screenWindow, cfg.lensRadius, cfg.focalDistance);
}

export std::unique_ptr<Film> makeFilm(const FilmConfig& cfg)
{
  return std::make_unique<Film>(cfg.resolution);
}

export std::unique_ptr<Sampler> makeSampler(const SamplerConfig& cfg, Int seed = 0)
{
  auto rng{ std::make_unique<PCG32>() };
  
  if (cfg.isStratified)
  {
    return std::make_unique<StratifiedSampler>(cfg.strata, cfg.isJitter, static_cast<Int64>(seed), std::move(rng));
  }

  const Int spp{ std::max<Int>(1, cfg.samplesPerPixel) };

  return std::make_unique<IndependentSampler>(spp, static_cast<UInt64>(seed), std::move(rng));
}

export std::unique_ptr<Integrator> makeIntegrator(const IntegratorConfig& cfg, CameraBase& camera, Sampler& sampler)
{
  return std::make_unique<PathIntegrator>(camera, sampler, cfg.maxDepth, cfg.useRR);
}