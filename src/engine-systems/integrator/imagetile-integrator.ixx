export module imagetileintegrator;

import integrator;
import sampler;
import camerabase;
import scene;
import types;
import renderprogress;
import parallel;
import engineconstructs;
import cameraconstructs;
import bounds;

// Camera and Sampler are held by reference, assume their lifetimes exceed the integrator (they’re owned by the engine).

export class ImageTileIntegrator : public Integrator 
{
public:
  ImageTileIntegrator(CameraBase&, Sampler&) noexcept;

  void render(const Scene&) override;
  float getCurrentProgress() const noexcept;
  void publishSnapshot();

protected:
  virtual void evaluatePixelSample(Point2i, Int, const Scene&, Sampler&) = 0;

  CameraBase& m_camera;
  Sampler& m_samplerPrototype;

  std::uint64_t m_snapshotSeq{ 0 };


private:
  void renderSampleWaves(const Scene& scene, RenderProgress& progress, const Bounds2i& pixelBounds, const Int samplesPerPixel);
};

ImageTileIntegrator::ImageTileIntegrator(CameraBase& camera, Sampler& sampler) noexcept : m_camera { camera }, m_samplerPrototype{ sampler } {}

void ImageTileIntegrator::render(const Scene& scene)
{
  Int nWaves{};
  const auto& pixelRes{ m_camera.getFilm().getFilmResolution() };
  const Bounds2i pixelBounds{ Point2i{}, Point2i{ static_cast<Int>(pixelRes[0]), static_cast<Int>(pixelRes[1]) } };
  const Int samplesPerPixel{ m_samplerPrototype.getSPP() };
  
  for (Int startingSampleIdx{}, SPPForWave{ 1 }; startingSampleIdx < samplesPerPixel; startingSampleIdx += SPPForWave, SPPForWave = std::min<Int>(64, SPPForWave * 2))
  {
    ++nWaves;
  }

  RenderProgress progress{ parallelTileCount(pixelBounds) * nWaves, 40 };
  progress.begin();
  
  renderSampleWaves(scene, progress, pixelBounds, samplesPerPixel);

  progress.done();
}

float ImageTileIntegrator::getCurrentProgress() const noexcept
{
  return 0.0f;
}

void ImageTileIntegrator::publishSnapshot()
{
  const auto img{ m_camera.getFilm().toImageU8(ColorEncoding::sRGB, Float{ 1 }) };

  FrameSnapshot snap{ std::move(img), getCurrentProgress(), ++m_snapshotSeq };

  if (m_displayConsumer) m_displayConsumer(std::move(snap));
}

void ImageTileIntegrator::renderSampleWaves(const Scene& scene, RenderProgress& progress, const Bounds2i& pixelBounds, const Int samplesPerPixel)
{
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
    publishSnapshot();
  }
}



