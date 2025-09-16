export module imagetileintegrator;

import integrator;
import sampler;
import camerabase;
import world_object;
import types;

// For now, tile = row. Easy to extend later to progressive/Morton tiles.
// Camera and Sampler are held by reference, assume their lifetimes exceed the integrator (they’re owned by the engine).
// EvaluatePixelSample() is left pure virtual: derived classes define what to do with each sample (e.g. generate a ray, call Li).

export class ImageTileIntegrator : public Integrator 
{
public:
  ImageTileIntegrator(CameraBase&, Sampler&) noexcept;

  void render(const WorldObject&) override;

protected:
  virtual void evaluatePixelSample(Point2i, Int, const WorldObject&, Sampler&) = 0;

  CameraBase& m_camera;
  Sampler& m_samplerPrototype;
};

ImageTileIntegrator::ImageTileIntegrator(CameraBase& camera, Sampler& sampler) noexcept : m_camera{ camera }, m_samplerPrototype{ sampler } {}

void ImageTileIntegrator::render(const WorldObject& world)
{
  const auto& res{ m_camera.film().getFilmResolution() };

  for (Idx y{}; y < res[1]; ++y)
  {
    for (Idx x{}; x < res[0]; ++x)
    {
      for (Idx s{}; s < m_samplerPrototype.getSPP(); ++s)
      {
        const Point2i pPixel{ x, y };
        m_samplerPrototype.startPixelSample(pPixel, static_cast<Int>(s), 0);        
        evaluatePixelSample(pPixel, static_cast<Int>(s), world, m_samplerPrototype);
      }
    }
  }
}