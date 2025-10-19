export module rng;

import std;
import concepts;
import mathutil;
import mathfp;
import mathconstants;
import types;

export class RNG
{
public:
  RNG() = default;

  virtual void setSequence(UInt64 sequence, UInt64 offset) noexcept = 0;
  virtual void advance(Int64 delta) noexcept = 0;
  virtual std::unique_ptr<RNG> clone() const = 0;
  virtual void setSeedAndStream(UInt64 seed, UInt64 stream) noexcept = 0;

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

  virtual UInt32 nextU32() noexcept = 0;
  virtual UInt64 nextU64() noexcept = 0;
};

// Return an integer by cardinality. Pending deeper understanding.
template<IntegralArithmetic T>
T RNG::uniform(T cardinality)
{
  //// Take the two's complement of the cardinality to get an unbiased distribution
  //T threshold{ static_cast<T>(~cardinality + 1u) % cardinality };
  //
  //// Use rejection sampling to only take values above this threshold modulo the cardinality, hence returning a conformant integer 
  //for (;;) 
  //{ 
  //  T r{ uniform<T>() }; 
  //  if (r >= threshold) return r % cardinality;
  //}

  using U = std::make_unsigned_t<T>;

  const U ucard{ static_cast<U>(cardinality) };
  const U threshold{ modPos<U>(-ucard, ucard) };

  for (;;) 
  {
    const U r{ uniform<U>() };
    if (r >= threshold) return static_cast<T>(r % ucard);
  }
}

template<>
UInt32 RNG::uniform<UInt32>()
{
  return nextU32();
}

template<>
UInt64 RNG::uniform<UInt64>()
{
  return (static_cast<UInt64>(nextU32()) << 32) | nextU32();
}

template<>
Int32 RNG::uniform<Int32>()
{
  UInt32 u{ uniform<UInt32>() };
  Int32 s{ std::bit_cast<Int32>(u) };

  return s;
}

template<>
Int64 RNG::uniform<Int64>()
{
  return static_cast<Int64>(uniform<UInt64>());
}

template<>
float RNG::uniform<float>()
{
  return std::min(oneMinusEpsFloat, uniform<UInt32>() * 0x1p-32f);
}

template<>
double RNG::uniform<double>()
{
  return std::min(oneMinusEpsDouble, static_cast<double>(uniform<UInt64>()) * 0x1p-64);
}