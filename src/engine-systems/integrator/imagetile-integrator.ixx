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
import dirtylatch;

// Camera and Sampler are held by reference, assume their lifetimes exceed the integrator (they’re owned by the engine).

export class ImageTileIntegrator : public Integrator 
{
public:
  ImageTileIntegrator(const RuntimeComponents& renderRuntimeComponents, CameraBase&, Sampler&) noexcept;

  void render(const Scene&) override;

protected:
  virtual void evaluatePixelSample(Point2i, Int, const Scene&, Sampler&) = 0;

  CameraBase& m_camera;
  Sampler& m_samplerPrototype;

  std::vector<std::uint8_t> m_displayBytes{};

private:
  void renderSampleWaves(const Scene& scene, RenderProgress& progress, const Bounds2i& pixelBounds, const Int samplesPerPixel);
  void writeToDisplaySink();
  void updateRuntimeSharedState(const Int samplesPerPixel);
};

ImageTileIntegrator::ImageTileIntegrator(const RuntimeComponents& renderRuntimeComponents, CameraBase& camera, Sampler& sampler) noexcept : Integrator{ renderRuntimeComponents }, m_camera { camera }, m_samplerPrototype{ sampler } {}

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

void ImageTileIntegrator::renderSampleWaves(const Scene& scene, RenderProgress& progress, const Bounds2i& pixelBounds, const Int samplesPerPixel)
{
  for (Int waveStartIdx{}, waveSize{ 1 }; waveStartIdx < samplesPerPixel; waveStartIdx = std::min(samplesPerPixel, waveStartIdx + waveSize), waveSize = std::min<Int>(64, waveSize * 2))
  {
    const auto renderTileWave = [&](const Bounds2i& tile)
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

            updateRuntimeSharedState(samplesPerPixel);
          }
        }
      }
      progress.tileDone();
    };

    parallelFor2D(pixelBounds, renderTileWave);

    writeToDisplaySink();
  }
}

void ImageTileIntegrator::updateRuntimeSharedState(const Int samplesPerPixel)
{
  auto& runtimeSharedState{ m_runtimeComponents.runtimeSharedState->get() };

  const std::uint64_t samplesCompleted{ runtimeSharedState.samplesCompleted.fetch_add(1, std::memory_order_relaxed) };
  const std::uint64_t totalSamples{ std::uint64_t(m_camera.getFilm().getFilmResolution()[0]) * m_camera.getFilm().getFilmResolution()[1] * samplesPerPixel };

  if ((samplesCompleted & 0xFFFF) == 0)
  {
    runtimeSharedState.progressUnitNormalized.store(static_cast<float>(samplesCompleted) / static_cast<float>(totalSamples), std::memory_order_relaxed);
  }
}

void ImageTileIntegrator::writeToDisplaySink()
{
  if (m_runtimeComponents.displayBytesArr && m_runtimeComponents.displayMutex)
  {
    const auto& rgba{ m_camera.getFilm().bakeDisplay() };

    auto& displayBytes{ m_runtimeComponents.displayBytesArr->get() };

    auto& displayMutex{ m_runtimeComponents.displayMutex->get() };
    {
      std::lock_guard<std::mutex> lock(displayMutex);
      displayBytes = std::move(rgba);
    }

    if (m_runtimeComponents.dirtyLatch) m_runtimeComponents.dirtyLatch->get().publish();
  }
}



