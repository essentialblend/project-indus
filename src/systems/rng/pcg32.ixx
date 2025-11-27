export module indus.rng.pcg32;

import indus.rng;

export class PCG32 final : public RNG
{
public:
  PCG32() = default;

  explicit PCG32(UInt64 seed, UInt64 stream) noexcept;

  void setSeedAndStream(UInt64 seed, UInt64 stream) noexcept override;
  void setSequence(UInt64 sequence, UInt64 offset) noexcept override;
  void advance(Int64 iDelta) noexcept override;

  std::unique_ptr<RNG> clone() const override;
  [[nodiscard]] virtual std::string toString() const override;

protected:
  UInt32 nextU32() noexcept override;
  UInt64 nextU64() noexcept override;

private:
  static constexpr UInt64 m_kMult{ 0x5851F42D4C957F2Dull };
  UInt64 m_state{ 0x853C49E6748FEA9Bull };
  UInt64 m_increment{ 0xDA3E39CB94B95BDBull };
};

PCG32::PCG32(UInt64 seed, UInt64 stream) noexcept
{
  setSeedAndStream(seed, stream);
}

void PCG32::setSeedAndStream(UInt64 seed, UInt64 stream) noexcept
{
  m_state = 0u;
  m_increment = (stream << 1u) | 1u;
  nextU32();
  m_state += seed;
  nextU32();
}

void PCG32::setSequence(UInt64 sequence, UInt64 offset) noexcept
{
  setSeedAndStream(sequence, sequence);
  advance(static_cast<Int64>(offset));
}

void PCG32::advance(Int64 iDelta) noexcept
{
  UInt64 curMult{ m_kMult }; 
  UInt64 curPlus{ m_increment };
  UInt64 accMult{ 1u };
  UInt64 accPlus{ 0u };
  UInt64 delta{ static_cast<UInt64>(iDelta) };
  
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
  
  m_state = (accMult * m_state) + accPlus;
}

std::unique_ptr<RNG> PCG32::clone() const
{
  auto p{ std::make_unique<PCG32>() };

  p->m_state = m_state; 
  p->m_increment = m_increment;
  
  return p;
}

std::string PCG32::toString() const
{
  return "PCG32";
}

UInt32 PCG32::nextU32() noexcept
{
  const UInt64 old{ m_state };
  
  m_state = old * m_kMult + m_increment;

  const UInt32 x{ static_cast<UInt32>(((old >> 18u) ^ old) >> 27u) };
  const UInt32 r{ static_cast<UInt32>(old >> 59u) };
  
  return (x >> r) | (x << ((~r + 1u) & 31u));
}

UInt64 PCG32::nextU64() noexcept
{
  return ((static_cast<UInt64>(nextU32()) << 32) | static_cast<UInt64>(nextU32()));
}
