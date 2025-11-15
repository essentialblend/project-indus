export module stratifiedsampler;

import std;
import sampler;
import samplingconstructs;
import samplingutil;
import rng;
import types;


export class StratifiedSampler final : public Sampler
{
public:
  explicit StratifiedSampler(Strata2D strata, bool jitter, Int64 seed, std::unique_ptr<RNG> rngPrototype) noexcept;

  [[nodiscard]] Int getSPP() const noexcept override;

  void startPixelSample(Point2i pPixel, Int sampleIndex, Int startingDimension = 0) override;
  [[nodiscard]] std::unique_ptr<Sampler> clone() const override;

  Float get1D() override;
  Point2f get2D() override;

  Point2f getPixel2D() override;

  [[nodiscard]] virtual std::string toString() const override;
  virtual const RNG& getRNG() const noexcept override;

private:
  std::unique_ptr<RNG> m_rng{};
  Int m_dimension{};
  Int m_spp{ 5 };
  Point2i m_currentPixel{};
  Strata2D m_strata{ 1 };
  Int64 m_sampleIndex{};
  Float m_invNX{};
  Float m_invNY{};
  bool m_jitter{ true };
  Int64 m_seed{};
};

StratifiedSampler::StratifiedSampler(Strata2D strata, bool jitter, Int64 seed, std::unique_ptr<RNG> rngPrototype) noexcept : m_rng{ std::move(rngPrototype) }, m_currentPixel{}
  , m_strata{ strata }, m_sampleIndex{ 0 }, m_invNX{ strata.NX ? (1.f / Float(strata.NX)) : 0.f }, m_invNY{ strata.NY ? (1.f / Float(strata.NY)) : 0.f }, m_jitter{ jitter }, m_seed{ seed }, m_dimension{ 0 } 
{
  if (m_strata.NX <= 0 || m_strata.NY <= 0) 
  {
    m_strata.NX = 1;
    m_strata.NY = 1;
  }

  m_invNX = Float{ 1.0 } / static_cast<Float>(m_strata.NX);
  m_invNY = Float{ 1.0 } / static_cast<Float>(m_strata.NY);

  const Int64 prod{ static_cast<Int64>(m_strata.NX) * static_cast<Int64>(m_strata.NY) };
  const Int64 cap{ Int64{ std::numeric_limits<Int>::max() } };

  m_spp = static_cast<Int>(prod > cap ? cap : prod);
}

Int StratifiedSampler::getSPP() const noexcept
{
  return m_spp;
}

std::string StratifiedSampler::toString() const
{
  return "stratified";
}

const RNG& StratifiedSampler::getRNG() const noexcept
{
  return *m_rng;
}

void StratifiedSampler::startPixelSample(Point2i pPixel, Int sampleIndex, Int startingDimension)
{
  m_currentPixel = pPixel;
  m_sampleIndex = sampleIndex;
  m_dimension = startingDimension;

  const UInt64 seq{ hash(pPixel, m_seed) };

  const UInt64 off{ static_cast<UInt64>(static_cast<UInt32>(sampleIndex)) * 65536ull + static_cast<UInt64>(static_cast<UInt32>(startingDimension)) };

  m_rng->setSequence(seq, off);
}

Float StratifiedSampler::get1D()
{
  const UInt64 h{ hash(m_currentPixel, m_dimension, m_seed) };
  const Int stratum{ permuteElement(static_cast<Int>(m_sampleIndex), m_spp, h) };
  
  ++m_dimension;

  const Float delta{ m_jitter ? m_rng->uniform<Float>() : Float{ 0.5 } };
  const Float v{ (static_cast<Float>(stratum) + delta) / static_cast<Float>(m_spp) };

  return v;
}

Point2f StratifiedSampler::get2D()
{
  const UInt64 h{ hash(m_currentPixel, m_dimension, m_seed) };
  const Int stratum{ permuteElement(static_cast<Int>(m_sampleIndex), m_spp, h) };

  const Int x{ stratum % m_strata.NX };
  const Int y{ stratum / m_strata.NX };

  const Float dx{ m_jitter ? m_rng->uniform<Float>() : Float{ 0.5 } };
  const Float dy{ m_jitter ? m_rng->uniform<Float>() : Float{ 0.5 } };

  m_dimension += 2;

  return Point2f{ (static_cast<Float>(x) + dx) * m_invNX, (static_cast<Float>(y) + dy) * m_invNY };
}

Point2f StratifiedSampler::getPixel2D()
{
  return get2D();
}

std::unique_ptr<Sampler> StratifiedSampler::clone() const
{
  auto s{ std::make_unique<StratifiedSampler>(m_strata, m_jitter, m_seed, m_rng->clone()) };

  s->m_spp = m_spp;
  s->m_invNX = m_invNX;
  s->m_invNY = m_invNY;

  s->m_currentPixel = {};
  s->m_sampleIndex = 0;
  s->m_dimension = 0;

  return s;
}
