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
  Int m_spp{ 1 };
  Int m_sampleIndex{};
  std::uint64_t m_baseSeed{};

  std::uint32_t m_remainingLogs{ 10 };
  std::uint32_t m_logIndex{};
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

  //// The debugging steps that solved the problem
  //
  //// a
  ////m_rng->setSequence(static_cast<std::uint64_t>(m_baseSeed), std::uint64_t{ 0 });

  ////// b
  // const std::uint64_t seq = mixBits(static_cast<std::uint64_t>(m_baseSeed) ^ static_cast<std::uint64_t>(static_cast<std::uint32_t>(sampleIndex)));
  // m_rng->setSequence(seq, std::uint64_t{ 0 });

  const uint64_t pixelHash = hash(pPixel, m_baseSeed);
  const uint64_t seq = mixBits(pixelHash ^ static_cast<std::uint64_t>(sampleIndex));

  // Use startingDimension for the offset, not a stride
  m_rng->setSequence(seq, static_cast<std::uint64_t>(startingDimension));
}

std::unique_ptr<Sampler> IndependentSampler::clone() const
{
  auto cloned{ std::make_unique<IndependentSampler>(m_spp, m_baseSeed, m_rng->clone()) };
  cloned->m_currentPixel = m_currentPixel; cloned->m_sampleIndex = m_sampleIndex;

  return cloned;
}

Float IndependentSampler::get1D()
{
  return m_rng->uniform<Float>();
}

Point2f IndependentSampler::get2D()
{
  /*if (m_remainingLogs > 0 && m_logIndex < 10) 
  {
    std::print("px={},{} s={} get2D[{}]=({:.9f},{:.9f})\n",
      m_currentPixel[0], m_currentPixel[1], m_sampleIndex, m_logIndex, u, v);
    --m_remainingLogs;
    ++m_logIndex;
  }*/
  return { get1D(), get1D() };
}

Point2f IndependentSampler::getPixel2D()
{
  return get2D();
}
