export module imagetileintegrator;

import integrator;
import sampler;
import camerabase;
import scene;
import types;
import parallelutil;
import engineconstructs;
import cameraconstructs;
import bounds;
import systemstatscollector;
import statsaccumulator;

// Camera and Sampler are held by reference, assume their lifetimes exceed the integrator (they’re owned by the engine).

export class ImageTileIntegrator : public Integrator 
{
public:
  ImageTileIntegrator(CameraBase&, Sampler&) noexcept;

  void render(const Scene&) override;
  float getCurrentProgress() const noexcept;
  void publishSnapshot();

  [[nodiscard]] std::string getSchedulerString() const override;

protected:
  virtual void evaluatePixelSample(Point2i, Int, const Scene&, Sampler&) = 0;

  CameraBase& m_camera;
  Sampler& m_samplerPrototype;

  std::uint64_t m_snapshotSeq{ 0 };


private:
  std::atomic<std::uint64_t> m_samplesDone{};
  std::uint64_t m_totalSamples{};

  void renderSampleWaves(const Scene& scene, const Bounds2i& pixelBounds, const Int samplesPerPixel);
  void notifySampleDone();
};

ImageTileIntegrator::ImageTileIntegrator(CameraBase& camera, Sampler& sampler) noexcept : m_camera{ camera }, m_samplerPrototype{ sampler } {}

void ImageTileIntegrator::render(const Scene& scene)
{
  const Int samplesPerPixel{ m_samplerPrototype.getSPP() };
  
  StatsAccumulator::reset(ParallelSystems::getEngineThreadPool().getSize() + 1);

  Int nWaves{};

  {
    SystemStatsCollector _statsCollector{};
    const auto& pixelRes{ m_camera.getFilm().getFilmResolution() };
    const Bounds2i pixelBounds{ Point2i{}, Point2i{ static_cast<Int>(pixelRes[0]), static_cast<Int>(pixelRes[1]) } };

    for (Int startingSampleIdx{}, SPPForWave{ 1 }; startingSampleIdx < samplesPerPixel; startingSampleIdx += SPPForWave, SPPForWave = std::min<Int>(64, SPPForWave * 2))
    {
      ++nWaves;
    }

    renderSampleWaves(scene, pixelBounds, samplesPerPixel);
  }
  m_samplesDone.store(m_totalSamples, std::memory_order_relaxed);
  Image img{ m_camera.getFilm().toImageU8(ColorEncoding::sRGB, 1.0f) };
  
  m_renderStats = StatsAccumulator::finalize();
  m_renderStats.spp = static_cast<std::uint64_t>(samplesPerPixel);
  FrameSnapshot snap{ std::move(img), 1.0f, ++m_snapshotSeq, m_renderStats };
  
  if (m_displayConsumer) m_displayConsumer(std::move(snap));
}

float ImageTileIntegrator::getCurrentProgress() const noexcept
{
  const auto done{ m_samplesDone.load(std::memory_order_relaxed) };

  return m_totalSamples ? static_cast<float>(done) / static_cast<float>(m_totalSamples)
    : Float{};
}

void ImageTileIntegrator::publishSnapshot()
{
  const auto img{ m_camera.getFilm().toImageU8(ColorEncoding::sRGB, Float{ 1 }) };

  FrameSnapshot snap{ std::move(img), getCurrentProgress(), ++m_snapshotSeq };

  if (m_displayConsumer) m_displayConsumer(std::move(snap));
}

std::string ImageTileIntegrator::getSchedulerString() const
{
  return "image-tile (1, 1, 2, 4, ...)";
}

void ImageTileIntegrator::renderSampleWaves(const Scene& scene, const Bounds2i& pixelBounds, const Int samplesPerPixel)
{
  const auto& res{ m_camera.getFilm().getFilmResolution() };

  m_samplesDone.store(0, std::memory_order_relaxed);

  m_totalSamples = std::uint64_t(res[0]) * std::uint64_t(res[1]) * std::uint64_t(samplesPerPixel);

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
            notifySampleDone();
          }
        }
      }
    };

    parallelFor2D(pixelBounds, renderTile);
    publishSnapshot();
  }
}

void ImageTileIntegrator::notifySampleDone()
{
  const auto done{ m_samplesDone.fetch_add(1, std::memory_order_relaxed) + 1 };

  if ((done & 0xFFFFu) == 0u) 
  {
    const float p{ m_totalSamples ? float(done) / float(m_totalSamples) : 0.f };
    
    FrameSnapshot snap{ Image{}, p, ++m_snapshotSeq };

    if (m_displayConsumer) m_displayConsumer(std::move(snap));
  }
}



