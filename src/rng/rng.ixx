export module rng;

import std;
import concepts;

export class RNG
{
public:
  RNG() = default;

  virtual void setSequence(std::uint64_t sequence, std::uint64_t offset) noexcept = 0;
  virtual void advance(std::int64_t delta) noexcept = 0;
  virtual std::unique_ptr<RNG> clone() const = 0;
  virtual void setSeedAndStream(std::uint64_t seed, std::uint64_t stream) noexcept = 0;

  template<Arithmetic T> 
  T uniform();

  template<IntegralArithmetic T>
  T uniform(T bound);

  virtual ~RNG() = default;

protected:
  RNG(const RNG&) = delete;
  RNG(RNG&&) = delete;

  RNG& operator=(const RNG&) = delete;
  RNG& operator=(RNG&&) = delete;

  virtual std::uint32_t nextU32() noexcept = 0;
  virtual std::uint64_t nextU64() noexcept = 0;


  static float oneMinusEpsF();
  static double oneMinusEpsD();
};

// Return an integer by cardinality
template<IntegralArithmetic T>
T RNG::uniform(T cardinality)
{
  // Take the two's complement of the cardinality to get an unbiased distribution
  T threshold{ static_cast<T>(~cardinality + 1u) % cardinality };
  
  // Use rejection sampling to only take values above this threshold modulo the cardinality, hence returning a conformant integer 
  for (;;) 
  { 
    T r{ uniform<T>() }; 
    if (r >= threshold) return r % cardinality;
  }
}

template<>
std::uint32_t RNG::uniform<std::uint32_t>()
{
  return nextU32();
}

template<>
std::uint64_t RNG::uniform<std::uint64_t>()
{
  return (static_cast<std::uint64_t>(nextU32()) << 32) | nextU32();
}

template<>
std::int32_t RNG::uniform<std::int32_t>()
{
  std::uint32_t u{ uniform<std::uint32_t>() };
  std::int32_t s{ std::bit_cast<std::int32_t>(u) };

  return s;
}

template<>
std::int64_t RNG::uniform<std::int64_t>()
{
  return static_cast<std::int64_t>(uniform<std::uint64_t>());
}

template<>
float RNG::uniform<float>()
{
  return std::min(oneMinusEpsF(), uniform<std::uint32_t>() * 0x1p-32f);
}

template<>
double RNG::uniform<double>()
{
  return std::min(oneMinusEpsD(), static_cast<double>(uniform<std::uint64_t>()) * 0x1p-64);
}

float RNG::oneMinusEpsF()
{
  return std::nextafter(1.0f, 0.0f);
}

double RNG::oneMinusEpsD()
{
  return std::nextafter(1.0, 0.0);
}