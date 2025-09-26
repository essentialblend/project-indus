export module core_sampling_util;

import std;
import vector;
import types;
import constructs;
import point;
import onb;
import core_diag;

import <cassert>;

export
{
  [[nodiscard]] constexpr Point2f concentricSampleDisk(Point2f sample) noexcept
  {
    Float sX{ (2 * sample[0]) - 1};
    Float sY{ (2 * sample[1]) - 1};

    if (sX == 0 && sY == 0) return Point2f{};

    Float r{}; Float theta{};

    if (std::abs(sX) > std::abs(sY))
    {
      r = sX;
      theta = (std::numbers::pi_v<Float> / Float(4.0)) * (sY / sX);
    }
    else
    {
      r = sY;
      theta = (std::numbers::pi_v<Float> / Float(2.0)) - ((std::numbers::pi_v<Float> / Float(4.0)) * (sX / sY));
    }

    return Point2f{ r * std::cos(theta), r * std::sin(theta) };
  }

  [[nodiscard]] Vec3f genCosineWeightedHemisphereVec(Point2f sample) noexcept
  {
    Point2f d = concentricSampleDisk(sample);

    // Lift onto hemisphere with cosine weighted distribution
    Float z{ static_cast<Float>(std::sqrt(std::max(0.0, 1.0 - d[0] * d[0] - d[1] * d[1]))) };

    return Vec3f{ d[0], d[1], z };
  }

  std::uint64_t mixBits(std::uint64_t v) noexcept 
  {
    v ^= (v >> 31);
    v *= 0x7fb5d329728ea185ULL;
    v ^= (v >> 27);
    v *= 0x81dadef4bc2dd44dULL;
    v ^= (v >> 33);
    return v;
  }

  [[nodiscard]] std::uint64_t hash(Point2i p, std::uint64_t seed)
  {
    // Pack x and y into one 64-bit integer
    std::uint64_t h = (static_cast<std::uint64_t>(static_cast<std::uint32_t>(p[0])) << 32) | static_cast<std::uint64_t>(static_cast<std::uint32_t>(p[1]));
    // Mix with the seed
    h ^= seed;
    // Use mixBits as the hash function
    return mixBits(h);
  }

  [[nodiscard]] UInt64 hash(Point2i p, Int dimension, UInt64 seed) noexcept
  {
    UInt64 h = (static_cast<UInt64>(static_cast<UInt32>(p[0])) << 32) | static_cast<UInt64>(static_cast<UInt32>(p[1]));
    h ^= seed ^ 0x9E3779B97F4A7C15ull;
    h ^= static_cast<UInt64>(dimension) * 0xBF58476D1CE4E5B9ull;
    return mixBits(h);
  }

  [[nodiscard]] Int permuteElement(Int i, Int n, UInt64 seed) noexcept
  {
    if (n <= 1) return 0;

    auto mix = [](std::uint64_t x) noexcept { return mixBits(x); };

    UInt64 h{ mix(seed ^ std::uint64_t(n) * 0x9E3779B97F4A7C15ull) };

    auto gcd = [](Int a, Int b) noexcept 
    {
      while (b) 
      { 
        Int t = a % b; a = b; b = t; 
      }
      return a < 0 ? -a : a;
    };

    Int a{ static_cast<Int>((h | 1ull) % static_cast<UInt64>(n)) };
    
    if (a == 0) a = 1;
    
    while (gcd(a, n) != 1) a = (a + 1) % n;
    
    Int b{ static_cast<Int>(mix(h + 0xD1B54A32D192ED03ull) % static_cast<UInt64>(n)) };
    
    return static_cast<Int>((static_cast<Int64>(a) * i + b) % n);
  }

  [[nodiscard]] std::optional<RefractResult> refractLocal(const Vec3f& unitW_oLocal, const Float eta) noexcept
  {
    if (!isFinite(unitW_oLocal) || !isFinite(eta)) return std::nullopt;

    const Float cosineO{ std::clamp(unitW_oLocal[2], Float(-1.0), Float(1.0)) };
    const Float sine2O{ std::max(Float(0.0), Float(1.0 - (cosineO * cosineO))) };
    const Float sine2T{ eta * eta * sine2O };

    if (sine2T >= Float(1.0)) return std::nullopt;

    const Float cosineT{ std::sqrt(std::max(Float(0.0), Float(1.0 - sine2T))) };
    const Float signZ{ cosineO > Float(0.0) ? Float(-1.0) : Float(1.0) };

    Vec3f unitW_iLocal{ -eta * unitW_oLocal[0], -eta * unitW_oLocal[1], signZ * cosineT };
    unitW_iLocal = normalize(unitW_iLocal);

    if (!isFinite(unitW_iLocal)) return std::nullopt;

    return RefractResult{ unitW_iLocal, eta };
  }

}