export module indus.integrator.imagetile;

import indus.core.types;
import indus.core.geom.bounds;

import indus.camera.base;
import indus.camera.constructs;

import indus.integrator.base;

import indus.stats.accumulator;
import indus.stats.system_stats_collector;

import indus.sampler.base;

import indus.utilities.parallel;

// Camera and Sampler are held by reference; their lifetimes must exceed the integrator.

export class ImageTileIntegrator : public Integrator 
{
public:
  ImageTileIntegrator(CameraBase&, Sampler&) noexcept;

  void render(const Scene&, std::stop_token) override;
  Float getCurrentProgress() const noexcept;
  void publishSnapshot();

  [[nodiscard]] std::string getSchedulerString() const override;

protected:
  virtual void evaluatePixelSample(Point2i, Int, const Scene&, Sampler&) = 0;

  CameraBase& m_camera;
  Sampler& m_samplerPrototype;

  std::atomic<UInt64> m_snapshotSeq{ 0 };


private:
  std::atomic<UInt64> m_samplesDone{};
  UInt64 m_totalSamples{};

  void renderSampleWaves(const Scene& scene, const Bounds2i& pixelBounds, const Int samplesPerPixel, std::stop_token stopToken);
  void notifySampleDone();
};

ImageTileIntegrator::ImageTileIntegrator(CameraBase& camera, Sampler& sampler) noexcept : m_camera{ camera }, m_samplerPrototype{ sampler } {}

void ImageTileIntegrator::render(const Scene& scene, std::stop_token stopToken)
{
  if (stopToken.stop_requested()) return;

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

    renderSampleWaves(scene, pixelBounds, samplesPerPixel, stopToken);
  }
  m_samplesDone.store(m_totalSamples, std::memory_order_relaxed);
  Image img{ m_camera.getFilm().toImageU8(ColorEncoding::sRGB, Float{ 1 }) };
  
  m_renderStats = StatsAccumulator::finalize();
  m_renderStats.spp = static_cast<UInt64>(samplesPerPixel);
  const UInt64 snapshotSequence{ m_snapshotSeq.fetch_add(1, std::memory_order_relaxed) + 1 };
  FrameSnapshot snap{ std::move(img), Float{ 1.0 }, snapshotSequence, m_renderStats };
  
  if (m_displayConsumer) m_displayConsumer(std::move(snap));
}

Float ImageTileIntegrator::getCurrentProgress() const noexcept
{
  const auto done{ m_samplesDone.load(std::memory_order_relaxed) };

  return m_totalSamples ? static_cast<Float>(done) / static_cast<Float>(m_totalSamples)
    : Float{};
}

void ImageTileIntegrator::publishSnapshot()
{
  const auto img{ m_camera.getFilm().toImageU8(ColorEncoding::sRGB, Float{ 1 }) };

  const UInt64 snapshotSequence{ m_snapshotSeq.fetch_add(1, std::memory_order_relaxed) + 1 };
  FrameSnapshot snap{ std::move(img), getCurrentProgress(), snapshotSequence };

  if (m_displayConsumer) m_displayConsumer(std::move(snap));
}

std::string ImageTileIntegrator::getSchedulerString() const
{
  return "image-tile (1, 1, 2, 4, ...)";
}

void ImageTileIntegrator::renderSampleWaves(const Scene& scene, const Bounds2i& pixelBounds, const Int samplesPerPixel, std::stop_token stopToken)
{
  const auto& res{ m_camera.getFilm().getFilmResolution() };

  m_samplesDone.store(0, std::memory_order_relaxed);

  m_totalSamples = UInt64(res[0]) * UInt64(res[1]) * UInt64(samplesPerPixel);

  for (Int waveStartIdx{}, waveSize{ 1 }; waveStartIdx < samplesPerPixel; waveStartIdx = std::min(samplesPerPixel, waveStartIdx + waveSize), waveSize = std::min<Int>(64, waveSize * 2))
  {
    const auto renderTile = [&](const Bounds2i& tile)
    {
      if (stopToken.stop_requested()) return;
      const auto sampler{ m_samplerPrototype.clone() };

      const auto& minTileBounds{ tile.getMin() };
      const auto& maxTileBounds{ tile.getMax() };

      for (Int y{ minTileBounds[1] }; y < maxTileBounds[1] && !stopToken.stop_requested(); ++y)
      {
        for (Int x{ minTileBounds[0] }; x < maxTileBounds[0] && !stopToken.stop_requested(); ++x)
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
    const Float p{ m_totalSamples ? static_cast<Float>(done) / static_cast<Float>(m_totalSamples) : Float{ 0 } };
    
    const UInt64 snapshotSequence{ m_snapshotSeq.fetch_add(1, std::memory_order_relaxed) + 1 };
    FrameSnapshot snap{ Image{}, p, snapshotSequence };

    if (m_displayConsumer) m_displayConsumer(std::move(snap));
  }
}
