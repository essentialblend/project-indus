export module factory;

import std;
import engineconstructs;
import types;
import camerabase;
import filmbase;
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
import filter;
import dictionaries;
import rgbfilm;
import pixelsensor;

export std::unique_ptr<CameraBase> makeCamera(const CameraConfig& cfg, FilmBase& film)
{
  const Float startTime{ cfg.cameraShutter.shutterOpen };
  const Float endTime{ cfg.cameraShutter.shutterClose };

  const AnimatedTransform worldFromCamera{ cfg.cameraToWorld, startTime, cfg.cameraToWorld, endTime };

  const CameraTransform cameraTransform{ worldFromCamera, cfg.renderingSpace };

  return std::make_unique<PerspectiveCamera>(cameraTransform, cfg.cameraToWorld, cfg.cameraShutter, film, cfg.fovDegrees, cfg.screenWindow, cfg.lensRadius, cfg.focalDistance);
}

export std::unique_ptr<FilmBase> makeFilm(const FilmConfig& cfg)
{
  const RGBColorSpace& colorSpace{ ColorRegistry::sRGB() };

  std::unique_ptr<Filter> filter{ std::make_unique<BoxFilter>(cfg.filterRadius) };

  const PixelSensor sensor{ colorSpace.XYZFromRGB, colorSpace, cfg.imagingRatio };
  
  return std::make_unique<RGBFilm>(cfg.resolution, cfg.crop, cfg.diagonalMM,
    std::move(filter), sensor);
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