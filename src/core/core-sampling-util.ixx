export module core_sampling_util;

import std;
import vector;
import types;
import constructs;
import point;
import onb;

import <cassert>;

export
{
  [[nodiscard]] constexpr Point2f concentricSampleDisk(Point2f sample)
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

  [[nodiscard]] Vec3f genCosineWeightedHemisphereVec(Point2f sample)
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

}