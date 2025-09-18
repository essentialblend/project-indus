export module pcg32;

import std;
import rng;

export class PCG32 final : public RNG
{
public:
  PCG32() = default;

  explicit PCG32(std::uint64_t seed, std::uint64_t stream) noexcept;

  void setSeedAndStream(std::uint64_t seed, std::uint64_t stream) noexcept override;
  void setSequence(std::uint64_t sequence, std::uint64_t offset) noexcept override;
  void advance(std::int64_t iDelta) noexcept override;

  std::unique_ptr<RNG> clone() const override;

protected:
  std::uint32_t nextU32() noexcept override;
  std::uint64_t nextU64() noexcept override;

private:
  static constexpr std::uint64_t m_kMult{ 0x5851F42D4C957F2Dull };
  std::uint64_t m_state{ 0x853C49E6748FEA9Bull };
  std::uint64_t m_increment{ 0xDA3E39CB94B95BDBull };
};

PCG32::PCG32(std::uint64_t seed, std::uint64_t stream) noexcept
{
  setSeedAndStream(seed, stream);
}

void PCG32::setSeedAndStream(std::uint64_t seed, std::uint64_t stream) noexcept
{
  m_state = 0u;
  m_increment = (stream << 1u) | 1u;
  nextU32();
  m_state += seed;
  nextU32();
}

void PCG32::setSequence(std::uint64_t sequence, std::uint64_t offset) noexcept
{
  setSeedAndStream(sequence, sequence);
  advance(static_cast<std::int64_t>(offset));
}

void PCG32::advance(std::int64_t iDelta) noexcept
{
  std::uint64_t curMult{ m_kMult }; 
  std::uint64_t curPlus{ m_increment };
  std::uint64_t accMult{ 1u };
  std::uint64_t accPlus{ 0u };
  std::uint64_t delta{ static_cast<std::uint64_t>(iDelta) };
  
  while (delta) 
  { 
    if (delta & 1u) 
    { 
      accMult *= curMult; accPlus = accPlus * curMult + curPlus; 
    } 
    
    curPlus = (curMult + 1u) * curPlus; 
    curMult *= curMult; 
    delta >>= 1; 
  }
  
  m_state = accMult * m_state + accPlus;
}

std::unique_ptr<RNG> PCG32::clone() const
{
  auto p{ std::make_unique<PCG32>() };

  p->m_state = m_state; 
  p->m_increment = m_increment; 
  
  return p;
}

std::uint32_t PCG32::nextU32() noexcept
{
  const std::uint64_t old{ m_state };
  
  m_state = old * m_kMult + m_increment;

  const std::uint32_t x{ static_cast<std::uint32_t>(((old >> 18u) ^ old) >> 27u) };
  const std::uint32_t r{ static_cast<std::uint32_t>(old >> 59u) };
  
  return (x >> r) | (x << ((~r + 1u) & 31u));
}

std::uint64_t PCG32::nextU64() noexcept
{
  return ((static_cast<std::uint64_t>(nextU32()) << 32) | static_cast<std::uint64_t>(nextU32()));
}
