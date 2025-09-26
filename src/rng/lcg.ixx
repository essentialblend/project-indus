export module lcg;

import <cassert>;

import std;
import rng;
import constructs;
import types;
import core_sampling_util;

export class LCG final : public RNG 
{
public:
  LCG() = default;
  explicit LCG(std::uint64_t seed, std::uint64_t stream) noexcept;

  void setSeedAndStream(std::uint64_t seed, std::uint64_t stream) noexcept override;
  void setSequence(std::uint64_t sequence, std::uint64_t offset) noexcept override;
  void advance(std::int64_t deltaSteps) noexcept override;

  // Debug
  void setMultiplier(std::uint64_t multiplier) noexcept;
  void setIncrement(std::uint64_t increment) noexcept;
  void setStartingState(std::uint64_t startState) noexcept;

  std::unique_ptr<RNG> clone() const override;

protected:
  std::uint32_t nextU32() noexcept override;
  std::uint64_t nextU64() noexcept override;

private:
  // LCG comprises the recurrence: nextState = (m_kMultiplier * m_currentState) + m_increment
  //static constexpr std::uint64_t m_kMultiplier{ MMIXLCG::multiplier };
  std::uint64_t m_kMultiplier{ MMIXLCG::multiplier };
  std::uint64_t m_currentState{ 0ull };
  std::uint64_t m_increment{};
};

LCG::LCG(std::uint64_t seed, std::uint64_t stream) noexcept 
{ 
  setSeedAndStream(seed, stream); 
}

void LCG::setSeedAndStream(std::uint64_t seed, std::uint64_t stream) noexcept 
{
  // Set the seed provided by the user as the current place in the pRNG tape
  m_currentState = seed;
  // The stream helps pick a bijective increment and facilitates traversal over affine endomorphisms of Z/2^{64}Z, forcing it to be odd ensures full-period coverage. Any even increment will collapse to an even-only subgroup
  m_increment = (stream << 1u) | 1u;
}

void LCG::setSequence(std::uint64_t sequence, std::uint64_t offset) noexcept 
{
  setSeedAndStream(sequence, sequence);
  advance(static_cast<std::int64_t>(offset));
}

void LCG::advance(std::int64_t deltaSteps) noexcept
{
  std::uint64_t currentMultiplier{ m_kMultiplier };
  std::uint64_t currentIncrement{ m_increment };
  std::uint64_t accumulatedMultiplier{ 1ull };
  std::uint64_t accumulatedIncrement{ 0ull };
  std::uint64_t steps{ static_cast<std::uint64_t>(deltaSteps) };
  
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

void LCG::setMultiplier(std::uint64_t multiplier) noexcept
{
  m_kMultiplier = multiplier;
}

void LCG::setIncrement(std::uint64_t increment) noexcept
{
  m_increment = increment;
}

void LCG::setStartingState(std::uint64_t startState) noexcept
{
  m_currentState = startState;
}

std::unique_ptr<RNG> LCG::clone() const 
{
  auto out{ std::make_unique<LCG>() };
  
  out->m_currentState = m_currentState;
  out->m_increment = m_increment;
  
  return out;
}

std::uint32_t LCG::nextU32() noexcept 
{
  m_currentState = m_currentState * m_kMultiplier + m_increment;
  return static_cast<std::uint32_t>(m_currentState >> 32);
}

std::uint64_t LCG::nextU64() noexcept
{
  m_currentState = ((m_currentState * m_kMultiplier) + m_increment);
  return m_currentState;
}