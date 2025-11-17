export module rayintegrator;

import imagetileintegrator;

import camerabase;
import vector;
import ray;
import scene;
import sampler;
import types;
import colorrgb;
import cameraconstructs;
import samplingconstructs;
import mathfp;
import filter;
import threadutil;
import statsaccumulator;

export class RayIntegrator : public ImageTileIntegrator 
{
public:
  RayIntegrator(CameraBase&, Sampler&) noexcept;

protected:
  // Pure virtual radiance function: every concrete integrator defines this.
  virtual ColorRGB Li(const Ray&, const Scene&, Sampler&) = 0;

  // Implementation: generate camera sample, ray, call Li(), add to Film.
  void evaluatePixelSample(Point2i, [[maybe_unused]] Int, const Scene&, Sampler&) final override;
};

RayIntegrator::RayIntegrator(CameraBase& camera, Sampler& sampler) noexcept : ImageTileIntegrator{ camera, sampler } {}

void RayIntegrator::evaluatePixelSample(Point2i pPixel, [[maybe_unused]] Int sampleIndex, const Scene& scene, Sampler& sampler)
{
  const Point2f uPixel{ sampler.getPixel2D() };
  const Point2f uLens{ sampler.get2D() };

  auto& film{ m_camera.getFilm() };
  const auto& filter{ m_camera.getFilm().getFilter() };

  const auto fs{ filter.getFilterSampleAtOffset(uPixel) };

  const Point2f pFilm{ Float(pPixel[0]) + fs.pOffset[0] + Float{ 0.5 }, Float(pPixel[1]) + fs.pOffset[1] + Float{ 0.5 } };

  CameraSample cs{ pFilm, uLens, Float{} };

  CameraRay renderSpaceRay{ m_camera.generateRay(cs) };

  StatsAccumulator::recordCameraRay();

  if (isZero(renderSpaceRay.weight)) return;

  const ColorRGB L{ Li(renderSpaceRay.ray, scene, sampler) };

  const auto LEffective{ L * renderSpaceRay.weight * (1 /renderSpaceRay.exposureScale) };

  film.addSample(pFilm, LEffective, fs.weightOverPDF);
}