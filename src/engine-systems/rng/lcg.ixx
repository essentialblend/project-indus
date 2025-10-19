export module lcg;

import <cassert>;

import std;
import rng;
import miscconstructs;
import samplingconstructs;
import types;
import samplingutil;

export class LCG final : public RNG 
{
public:
  LCG() = default;
  explicit LCG(UInt64 seed, UInt64 stream) noexcept;

  void setSeedAndStream(UInt64 seed, UInt64 stream) noexcept override;
  void setSequence(UInt64 sequence, UInt64 offset) noexcept override;
  void advance(Int64 deltaSteps) noexcept override;

  // Debug
  void setMultiplier(UInt64 multiplier) noexcept;
  void setIncrement(UInt64 increment) noexcept;
  void setStartingState(UInt64 startState) noexcept;

  std::unique_ptr<RNG> clone() const override;

protected:
  UInt32 nextU32() noexcept override;
  UInt64 nextU64() noexcept override;

private:
  // LCG comprises the recurrence: nextState = (m_kMultiplier * m_currentState) + m_increment
  //static constexpr UInt64 m_kMultiplier{ MMIXLCG::multiplier };
  UInt64 m_kMultiplier{ MMIXLCG::multiplier };
  UInt64 m_currentState{ 0ull };
  UInt64 m_increment{};
};

LCG::LCG(UInt64 seed, UInt64 stream) noexcept 
{ 
  setSeedAndStream(seed, stream); 
}

void LCG::setSeedAndStream(UInt64 seed, UInt64 stream) noexcept 
{
  // Set the seed provided by the user as the current place in the pRNG tape
  m_currentState = seed;
  // The stream helps pick a bijective increment and facilitates traversal over affine endomorphisms of Z/2^{64}Z, forcing it to be odd ensures full-period coverage. Any even increment will collapse to an even-only subgroup
  m_increment = (stream << 1u) | 1u;
}

void LCG::setSequence(UInt64 sequence, UInt64 offset) noexcept 
{
  setSeedAndStream(sequence, sequence);
  advance(static_cast<Int64>(offset));
}

void LCG::advance(Int64 deltaSteps) noexcept
{
  UInt64 currentMultiplier{ m_kMultiplier };
  UInt64 currentIncrement{ m_increment };
  UInt64 accumulatedMultiplier{ 1ull };
  UInt64 accumulatedIncrement{ 0ull };
  UInt64 steps{ static_cast<UInt64>(deltaSteps) };
  
  while (steps) 
  {
    if (steps & 1ull) 
    {
      accumulatedMultiplier *= currentMultiplier;
      accumulatedIncrement = accumulatedIncrement * currentMultiplier + currentIncrement;
    }
    
    currentIncrement = (currentMultiplier + 1ull) * currentIncrement;
    currentMultiplier *= currentMultiplier;
    steps >>= 1u;
  }

  m_currentState = (accumulatedMultiplier * m_currentState) + accumulatedIncrement;

}

void LCG::setMultiplier(UInt64 multiplier) noexcept
{
  m_kMultiplier = multiplier;
}

void LCG::setIncrement(UInt64 increment) noexcept
{
  m_increment = increment;
}

void LCG::setStartingState(UInt64 startState) noexcept
{
  m_currentState = startState;
}

std::unique_ptr<RNG> LCG::clone() const 
{
  auto out{ std::make_unique<LCG>() };
  
  out->m_currentState = m_currentState;
  out->m_increment = m_increment;
  out->m_kMultiplier = m_kMultiplier;

  return out;
}

UInt32 LCG::nextU32() noexcept 
{
  m_currentState = m_currentState * m_kMultiplier + m_increment;
  return static_cast<UInt32>(m_currentState >> 32);
}

UInt64 LCG::nextU64() noexcept
{
  m_currentState = ((m_currentState * m_kMultiplier) + m_increment);
  return m_currentState;
}