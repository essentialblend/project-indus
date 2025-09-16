export module lcg;

import <cassert>;

import std;
import rng;
import constructs;
import types;
import core_sampling_util;

export class LCG : public RNG
{
public:
  LCG() = default;

  explicit LCG(std::uint64_t sequence, std::uint64_t offset = 0) noexcept;

  void setSequence(std::uint64_t sequence, std::uint64_t offset) noexcept override;
  void advance(std::int64_t delta) noexcept override;

  std::unique_ptr<RNG> clone() const override;
  
protected:
  std::uint32_t nextU32() noexcept override;
  std::uint64_t nextU64() noexcept override;

private:
  std::uint64_t m_state{};
  static constexpr std::uint64_t m_A{ MMIXLCG::multiplier };
  std::uint64_t m_increment{ 1 };
  
  static constexpr std::uint64_t odd(std::uint64_t x);
  static constexpr std::uint64_t invPow2(std::uint64_t a);
  
};

LCG::LCG(std::uint64_t sequence, std::uint64_t offset) noexcept
{
  setSequence(sequence, offset);
}

void LCG::setSequence(std::uint64_t sequence, std::uint64_t offset) noexcept 
{
  m_increment = ((sequence << 1) | 1ull); 
  m_state = 0ull;                                   
  if (offset) advance(static_cast<int64_t>(offset)); 
}

void LCG::advance(std::int64_t iDelta) noexcept 
{
  std::uint64_t delta{};
  std::uint64_t currMult{};
  std::uint64_t currInc{};

  if (iDelta >= 0) 
  {
    delta = static_cast<std::uint64_t>(iDelta);
    currMult = m_A;
    currInc = m_increment;
  }
  else 
  {
    delta = static_cast<std::uint64_t>(-iDelta);
    std::uint64_t ainv{ invPow2(m_A) };
    currMult = ainv;
    currInc = static_cast<std::uint64_t>(0 - ainv * m_increment);
  }

  std::uint64_t accMult{ 1 };
  std::uint64_t accInc{};

  while (delta) 
  {
    if (delta & 1) 
    { 
      accMult *= currMult; 
      accInc = (accInc * currMult) + currInc; 
    }
    currInc = (currMult + 1) * currInc;
    currMult *= currMult;
    delta >>= 1;
  }

  m_state = accMult * m_state + accInc;
}

std::unique_ptr<RNG> LCG::clone() const
{
  auto p = std::make_unique<LCG>();
  p->m_state = m_state;         
  p->m_increment = m_increment;
  return p;
}

std::uint32_t LCG::nextU32() noexcept 
{
  m_state = (m_A * m_state) + m_increment;
  const std::uint64_t z{ mixBits(m_state) };

  return static_cast<std::uint32_t>(z >> 32);
}

std::uint64_t LCG::nextU64() noexcept 
{
  const std::uint64_t hi{ static_cast<std::uint64_t>(nextU32()) << 32 };
  const std::uint64_t lo{ static_cast<std::uint64_t>(nextU32()) };

  return hi | lo;
}

constexpr std::uint64_t LCG::odd(std::uint64_t x)
{
  return (x << 1) | 1ull;
}

constexpr std::uint64_t LCG::invPow2(std::uint64_t a)
{
  std::uint64_t x{ 1 };
  for (Idx i{}; i < 6; ++i)
  {
    x *= (2 - (a * x));
  }

  return x;
}