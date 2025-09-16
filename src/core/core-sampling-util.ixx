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

  /*[[nodiscard]] std::uint64_t splitMix64(std::uint64_t x)
  {
    x += 0x9e3779b97f4a7c15ull;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
    return x ^ (x >> 31);
  }*/

  //[[nodiscard]] std::uint64_t murmur64a(const void* key, size_t len, uint64_t seed) noexcept 
  //{
  //  const uint64_t m = 0xc6a4a7935bd1e995ULL; const int r = 47;
  //  uint64_t h = seed ^ (len * m);
  //  const uint64_t* data = static_cast<const uint64_t*>(key);
  //  const uint64_t* end = data + (len / 8);
  //  while (data != end) { uint64_t k = *data++; k *= m; k ^= k >> r; k *= m; h ^= k; h *= m; }
  //  const unsigned char* data2 = reinterpret_cast<const unsigned char*>(end);
  //  switch (len & 7) {
  //  case 7: h ^= uint64_t(data2[6]) << 48; [[fallthrough]]
  //  case 6: h ^= uint64_t(data2[5]) << 40; case 5: h ^= uint64_t(data2[4]) << 32;
  //  case 4: h ^= uint64_t(data2[3]) << 24; case 3: h ^= uint64_t(data2[2]) << 16;
  //  case 2: h ^= uint64_t(data2[1]) << 8;  case 1: h ^= uint64_t(data2[0]); h *= m;
  //  }
  //  h ^= h >> r; h *= m; h ^= h >> r; return h;
  //}

  std::uint64_t murmur64aWords(std::initializer_list<std::uint64_t> w, std::uint64_t seed) noexcept 
  {
    constexpr std::uint64_t m = 0xc6a4a7935bd1e995ULL; constexpr int r = 47;
    
    std::uint64_t h = seed ^ (std::uint64_t(w.size()) * 8ULL * m);
    
    for (std::uint64_t k : w) { k *= m; k ^= (k >> r); k *= m; h ^= k; h *= m; }
    
    h ^= (h >> r); h *= m; h ^= (h >> r); return h;
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

  //[[nodiscard]] std::uint64_t hash(Point2i p, std::uint64_t seed)
  //{
  //  const uint64_t w[] = {
  //  static_cast<std::uint64_t>(static_cast<std::uint32_t>(p[0])),
  //  static_cast<std::uint64_t>(static_cast<std::uint32_t>(p[1])),
  //  seed
  //  };

  //  std::uint64_t h = murmur64aWords({ w[0], w[1], w[2] }, /*seed*/0ull);
  //  return mixBits(h); 
  //}

  [[nodiscard]] std::uint64_t hash(Point2i p, std::uint64_t seed)
  {
    // Pack x and y into one 64-bit integer
    std::uint64_t h = (static_cast<std::uint64_t>(static_cast<std::uint32_t>(p[0])) << 32) |
      static_cast<std::uint64_t>(static_cast<std::uint32_t>(p[1]));
    // Mix with the seed
    h ^= seed;
    // Use mixBits as the hash function
    return mixBits(h);
  }

}