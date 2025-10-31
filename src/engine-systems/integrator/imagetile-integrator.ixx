export module imagetileintegrator;

import integrator;
import sampler;
import camerabase;
import scene;
import types;
import renderprogress;
import parallel;

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
  const Int samplesPerPixel{ m_samplerPrototype.getSPP() };
  Int nWaves{};

  const Bounds2i pixelBounds{ Point2i{}, Point2i{ static_cast<Int>(res[0]), static_cast<Int>(res[1]) } };
  
  RenderProgress progress{ parallelTileCount(pixelBounds) * nWaves, 40 };
  
  for (Int startingSampleIdx{}, SPPForWave{ 1 }; startingSampleIdx < samplesPerPixel; startingSampleIdx += SPPForWave, SPPForWave = std::min<Int>(64, SPPForWave * 2))
  {
    ++nWaves;
  }

  progress.begin();

  for (Int waveStartIdx{}, waveSize{ 1 }; waveStartIdx < samplesPerPixel; waveStartIdx = std::min(samplesPerPixel, waveStartIdx + waveSize), waveSize = std::min<Int>(64, waveSize * 2))
  {
    const auto renderTile = [&](const Bounds2i& tile)
    {
      const auto sampler{ m_samplerPrototype.clone() };
      
      const auto& minTileBounds{ tile.getMin() }; 
      const auto& maxTileBounds{ tile.getMax() };

      for (Int y{ minTileBounds[1] }; y < maxTileBounds[1]; ++y)
      {
        for (Int x{ minTileBounds[0] }; x < maxTileBounds[0]; ++x) 
        {
          const Point2i p{ x, y };

          for (Int sampleIdx{ waveStartIdx }; sampleIdx < std::min(samplesPerPixel, waveStartIdx + waveSize); ++sampleIdx)
          {
            sampler->startPixelSample(p, sampleIdx, 0);
            evaluatePixelSample(p, sampleIdx, scene, *sampler);
          }
        }
      }
      progress.tileDone();
    };
    
    parallelFor2D(pixelBounds, renderTile);
  }
  progress.done();
}

