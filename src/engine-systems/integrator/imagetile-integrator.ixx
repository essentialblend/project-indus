export module imagetileintegrator;

import integrator;
import sampler;
import camerabase;
import scene;
import types;
import scanlines;

// For now, tile = row. Easy to extend later to progressive/Morton tiles.
// Camera and Sampler are held by reference, assume their lifetimes exceed the integrator (they’re owned by the engine).

export class ImageTileIntegrator : public Integrator 
{
public:
  ImageTileIntegrator(CameraBase&, Sampler&) noexcept;

  void render(const Scene&) override;

protected:
  virtual void evaluatePixelSample(Point2i, Int, const Scene&, Sampler&) = 0;

  CameraBase& m_camera;
  Sampler& m_samplerPrototype;
};

ImageTileIntegrator::ImageTileIntegrator(CameraBase& camera, Sampler& sampler) noexcept : m_camera{ camera }, m_samplerPrototype{ sampler } {}

void ImageTileIntegrator::render(const Scene& scene)
{
  const auto& res{ m_camera.getFilm().getFilmResolution() };

  ScanlineProgress prog{ static_cast<Int>(res[1]), 40 };
  prog.begin();

  for (Idx row{}; row < res[1]; ++row)
  {
    for (Idx col{}; col < res[0]; ++col)
    {
      const Point2i pPixel{ col, row };
      
      for (Idx s{}; s < m_samplerPrototype.getSPP(); ++s)
      {
        m_samplerPrototype.startPixelSample(pPixel, static_cast<Int>(s), 0);        
        evaluatePixelSample(pPixel, static_cast<Int>(s), scene, m_samplerPrototype);
      }
    }
    prog.lineDone(static_cast<Int>(row));
  }
}