export module stratifiedsampler;

import std;
import sampler;
import constructs;
import core_sampling_util;
import rng;
import rngLCG;

import <cassert>;
import <cstdint>;

export class StratifiedSampler final : public Sampler 
{
public:
  explicit StratifiedSampler(Idx, bool, Int, std::unique_ptr<RNG>);

  void startPixelSample([[maybe_unused]] Point2i, Idx, [[maybe_unused]] Int dimension = 0) override;
 
  Float get1D() override;
  Point2f get2D() override;
  Idx getSPP() const noexcept override;

  void req1DArray(Int) override;
  void req2DArray(Int) override;
  std::span<const Float> getReq1DArray(Int) const override;
  std::span<const Point2f> getReq2DArray(Int) const override;

  std::unique_ptr<Sampler> clone(Idx) const override;
  Idx roundCount(Idx) const override;
  bool setSampleNumber(Idx) override;

private:
  Idx m_samplesPerPixel{};
  Point2i m_currentPixel{};
  Strata2D m_strata2D{};
  Idx m_currentSample{};
  Int m_dimension{};
  Int m_seed{};
  bool m_jittered{ true };
  std::unique_ptr<RNG> m_rng{};
  std::vector<Float> m_array1D{};
  std::vector<Point2f> m_array2D{};

};

StratifiedSampler::StratifiedSampler(Idx spp, bool jittered, Int seed = 0, std::unique_ptr<RNG> rng = nullptr) : m_samplesPerPixel{ spp }, m_jittered{ jittered }, m_seed{ seed }, m_rng{ rng ? std::move(rng) : std::make_unique<LCG>(seed, 0) }, m_strata2D{ factorSPP(m_samplesPerPixel) } {}

Idx StratifiedSampler::getSPP() const noexcept
{
  return m_samplesPerPixel;
}

void StratifiedSampler::startPixelSample([[maybe_unused]] Point2i pPixel, Idx sampleIndex, [[maybe_unused]] Int dimension)
{
  assert(sampleIndex < m_samplesPerPixel);
  m_currentPixel = pPixel;
  m_currentSample = sampleIndex;
  m_dimension = dimension;

  const std::uint64_t stream{ hashPixelDimension(pPixel, m_seed, 0) };
  m_rng->setStream(stream);
  m_rng->reseed(0);

  m_rng->jumpAhead(std::uint64_t(sampleIndex) * 65536ull + std::uint64_t(dimension));
}

Float StratifiedSampler::get1D()
{  
  assert(m_samplesPerPixel > 0);
  const std::uint64_t key{ hashPixelDimension(m_currentPixel, m_seed, m_dimension) };
  const std::uint32_t stratum{ permuteStratum(static_cast<std::uint32_t>(m_currentSample), static_cast<std::uint32_t>(m_samplesPerPixel), key) };

  const Float jitter = m_jittered ? m_rng->nextF32() : static_cast<Float>(0.5);
  
  ++m_dimension;

  return (static_cast<Float>(stratum) + jitter) / static_cast<Float>(m_samplesPerPixel);
}

Point2f StratifiedSampler::get2D()
{
  const std::uint64_t hash{ hashPixelDimension(m_currentPixel, static_cast<std::uint32_t>(m_seed), static_cast<std::uint32_t>(m_dimension)) };
  const std::uint32_t stratum{ permuteStratum(static_cast<std::uint32_t>(m_currentSample), static_cast<std::uint32_t>(m_samplesPerPixel), hash) };

  const Int sx{ static_cast<Int>(stratum % static_cast<std::uint32_t>(m_strata2D.NX)) };
  const Int sy{ static_cast<Int>(stratum / static_cast<std::uint32_t>(m_strata2D.NX)) };

  const Float dx{ m_jittered ? m_rng->nextF32() : static_cast<Float>(0.5) };
  const Float dy{ m_jittered ? m_rng->nextF32() : static_cast<Float>(0.5) };

  m_dimension += 2;

  return {(sx + dx) / static_cast<Float>(m_strata2D.NX), (sy + dy) / static_cast<Float>(m_strata2D.NY) };
}

void StratifiedSampler::req1DArray(Int count)
{
  m_array1D.resize(count);
  for (int i = 0; i < count; ++i) m_array1D[i] = get1D();
}

void StratifiedSampler::req2DArray(Int count)
{
  m_array2D.resize(count);
  for (int i = 0; i < count; ++i) m_array2D[i] = get2D();
}

std::span<const Float> StratifiedSampler::getReq1DArray(Int) const
{
  return m_array1D;
}

std::span<const Point2f> StratifiedSampler::getReq2DArray(Int) const
{
  return m_array2D;
}

std::unique_ptr<Sampler> StratifiedSampler::clone(Idx seed) const
{
  return std::make_unique<StratifiedSampler>(m_samplesPerPixel, m_jittered, static_cast<Int>(seed));
}

Idx StratifiedSampler::roundCount(Idx n) const
{
  return ((n + m_samplesPerPixel - 1) / m_samplesPerPixel) * m_samplesPerPixel;
}

bool StratifiedSampler::setSampleNumber(Idx sampleIndex)
{
  // Pending
  return true;
}
