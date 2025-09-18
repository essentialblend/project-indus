export module independentsampler;

import std;
import sampler;
import types;
import rng;
import core_sampling_util;

export class IndependentSampler final : public Sampler
{
public:
  IndependentSampler() = default;

  explicit IndependentSampler(Int spp, std::uint64_t baseSeed, std::unique_ptr<RNG> rngPrototype) noexcept;

  [[nodiscard]] Int getSPP() const noexcept override;

  void startPixelSample(Point2i pPixel, Int sampleIndex, Int startingDimension = 0) override;

  std::unique_ptr<Sampler> clone() const override;

  Float get1D() override;
  Point2f get2D() override;
  Point2f getPixel2D() override;

private:
  std::unique_ptr<RNG> m_rng{};
  Point2i m_currentPixel{};
  Int m_spp{ 5 };
  Int m_sampleIndex{};
  std::uint64_t m_baseSeed{};
};

IndependentSampler::IndependentSampler(Int spp, std::uint64_t baseSeed, std::unique_ptr<RNG> rngPrototype) noexcept : m_spp{ spp }, m_baseSeed{ baseSeed }, m_rng{ std::move(rngPrototype) } {}

Int IndependentSampler::getSPP() const noexcept
{
  return m_spp;
}

void IndependentSampler::startPixelSample(Point2i pPixel, Int sampleIndex, Int startingDimension)
{
  m_currentPixel = pPixel;
  m_sampleIndex = sampleIndex;

  const auto seq{ mixBits(hash(pPixel, m_baseSeed)) };
  
  m_rng->setSeedAndStream(seq, seq);
  
  const auto off{ (static_cast<std::uint64_t>(static_cast<std::uint32_t>(sampleIndex)) << 16) + static_cast<std::uint64_t>(static_cast<std::uint32_t>(startingDimension)) };
  
  m_rng->advance(static_cast<std::int64_t>(off));
}

std::unique_ptr<Sampler> IndependentSampler::clone() const
{
  auto cloned{ std::make_unique<IndependentSampler>(m_spp, m_baseSeed, m_rng->clone()) };
  
  cloned->m_currentPixel = m_currentPixel; 
  cloned->m_sampleIndex = m_sampleIndex;

  return cloned;
}

Float IndependentSampler::get1D()
{
  return m_rng->uniform<Float>();
}

Point2f IndependentSampler::get2D()
{
  return { get1D(), get1D() };
}

Point2f IndependentSampler::getPixel2D()
{
  return get2D();
}
