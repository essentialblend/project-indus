export module rayintegrator;

import imagetileintegrator;

import camerabase;
import vector;
import ray;
import world_object;
import sampler;
import types;
import colorrgb;
import constructs;

export class RayIntegrator : public ImageTileIntegrator 
{
public:
  RayIntegrator(CameraBase&, Sampler&) noexcept;

protected:
  // Pure virtual radiance function: every concrete integrator defines this.
  virtual ColorRGB Li(const Ray&, const WorldObject&, Sampler&) = 0;

  // Implementation: generate camera sample, ray, call Li(), add to Film.
  void evaluatePixelSample(Point2i, [[maybe_unused]] Int, const WorldObject&, Sampler&) final override;
};

RayIntegrator::RayIntegrator(CameraBase& camera, Sampler& sampler) noexcept : ImageTileIntegrator{ camera, sampler } {}

void RayIntegrator::evaluatePixelSample(Point2i pPixel, [[maybe_unused]] Int sampleIndex, const WorldObject& world, Sampler& sampler)
{
  Point2f uFilm{ sampler.get2D() };
  Point2f uLens{ sampler.get2D() };

  CameraSample cs{ Point2f{ static_cast<Float>(pPixel[0]) + uFilm[0], static_cast<Float>(pPixel[1]) + uFilm[1]}, uLens, Float{} };

  CameraRay renderSpaceRay{ m_camera.generateRay(cs) };
  if (renderSpaceRay.weight == Float{}) return;

  ColorRGB L{ Li(renderSpaceRay.ray, world, sampler) };
  m_camera.getFilm().addSample(cs.pFilm, L, renderSpaceRay.weight);
}