export module factory;

import std;
import constructs;
import types;
import camerabase;
import film;
import perspectivecamera;
import sampler;
import independentsampler;
import integrator;
import pathintegrator;
import pcg32;

export std::unique_ptr<CameraBase> makeCamera(const CameraConfig& cfg, Film& film)
{
  return std::make_unique<PerspectiveCamera>(cfg.cameraToWorld, cfg.cameraShutter, film, cfg.fovDegrees, cfg.screenWindow, cfg.lensRadius,cfg.focalDistance);
}

export std::unique_ptr<Film> makeFilm(const FilmConfig& cfg)
{
  // TODO filename for writeImage
  return std::make_unique<Film>(cfg.resolution);
}

export std::unique_ptr<Sampler> makeSampler(const SamplerConfig& cfg, Int seed = 0)
{
  auto rng{ std::make_unique<PCG32>() };
  return std::make_unique<IndependentSampler>(cfg.samplesPerPixel, static_cast<std::uint64_t>(seed), std::move(rng));
}

export std::unique_ptr<Integrator> makeIntegrator(const IntegratorConfig& cfg, CameraBase& camera, Sampler& sampler)
{
  return std::make_unique<PathIntegrator>(camera, sampler, cfg.maxDepth, cfg.useRR);
}