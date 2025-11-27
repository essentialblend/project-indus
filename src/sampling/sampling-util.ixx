export module indus.sampling.util;

import std;

import indus.core.types;
import indus.core.math.fp.ii;
import indus.core.math.trig.iii;
import indus.core.math.algebra.iv;
import indus.core.math.float_constants;

import indus.sampling.constructs;

export
{
  [[nodiscard]] constexpr Point2f concentricSampleDisk(Point2f sample) noexcept
  {
    // Take the sample and center it to a symmetric square spanning [-1, +1)
    Float sX{ (2 * sample[0]) - 1};
    Float sY{ (2 * sample[1]) - 1};

    if (sX == 0 && sY == 0) return Point2f{};

    Float r{}; Float theta{};

    // Find the dominant component, evaluate the quadrant and map the slope to an angle theta
    if (std::abs(sX) > std::abs(sY))
    {
      r = sX;
      theta = (kPiOver4) * (sY / sX);
    }
    else
    {
      r = sY;
      theta = (kPiOver2) - (kPiOver4 * (sX / sY));
    }

    // Form a polar vector scaling the radius by the cosine and the sine, getting us the components of the vector on the unit disk
    return Point2f{ r * std::cos(theta), r * std::sin(theta) };
  }

  [[nodiscard]] Vec3f genCosineWeightedHemisphereVec(Point2f sample) noexcept
  {
    Point2f d{ concentricSampleDisk(sample) };

    // Lift onto hemisphere with cosine weighted distribution
    //Float z{ safeSqrt(Float(1) - sqr(d[0]) - sqr(d[1])) };
    const Float negR2{ sumOfProducts(-d[0], d[0], -d[1], d[1]) };
    const Float z{ safeSqrt(clamp(negR2 + Float{ 1 }, Float{}, Float{ 1 })) };
    
    return Vec3f{ d[0], d[1], z };
  }

  UInt64 mixBits(UInt64 v) noexcept 
  {
    // XOR-shift by 31 bits
    v ^= (v >> 31);
    // Choose an odd constant to smear and propagate strong diffusion 
    v *= 0x7fb5d329728ea185ULL;

    // XOR-shift and smear twice more to compound diffusion
    v ^= (v >> 27);
    v *= 0x81dadef4bc2dd44dULL;

    v ^= (v >> 33);
    
    return v;
  }

  [[nodiscard]] UInt64 hash(Point2i p, UInt64 seed)
  {
    // Pack p[0] and p[1] into an Int64
    UInt64 h{ (static_cast<UInt64>(static_cast<UInt32>(p[0])) << 32) | static_cast<UInt64>(static_cast<UInt32>(p[1])) };
    
    // XOR-shift by seed to maintain determinism
    h ^= seed;
    
    // Use mixBits as the hash function
    return mixBits(h);
  }

  [[nodiscard]] UInt64 hash(Point2i p, Int dimension, UInt64 seed) noexcept
  {
    UInt64 h{ (static_cast<UInt64>(static_cast<UInt32>(p[0])) << 32) | static_cast<UInt64>(static_cast<UInt32>(p[1])) };
    h ^= seed ^ 0x9E3779B97F4A7C15ull;
    h ^= static_cast<UInt64>(dimension) * 0xBF58476D1CE4E5B9ull;
    return mixBits(h);
  }

  [[nodiscard]] Int permuteElement(Int i, Int n, UInt64 seed) noexcept
  {
    if (n <= 1) return 0;

    auto nextPow2 = [](UInt32 value)
    {
      value--;
      value |= value >> 1;
      value |= value >> 2;
      value |= value >> 4;
      value |= value >> 8;
      value |= value >> 16;

      return value + 1;
    };

    const UInt32 stratumCount{ static_cast<UInt32>(n) };
    const UInt32 powerOfTwoCeil{ nextPow2(stratumCount) };
    const UInt32 indexMask{ powerOfTwoCeil - 1u };

    UInt32 indexState{ static_cast<UInt32>(i) };
    UInt64 streamSeed{ static_cast<UInt64>(seed) };

    for (;;)
    {
      const UInt32 permutedCandidate{ static_cast<UInt32>(mixBits(static_cast<UInt64>(indexState) ^ streamSeed)) & indexMask };

      if (permutedCandidate < stratumCount) return static_cast<Int>(permutedCandidate);

      indexState = permutedCandidate;
      streamSeed += 0x9E3779B97F4A7C15ull;
    }
  }

  [[nodiscard]] std::optional<RefractResult> refractLocal(const Vec3f& unitW_oLocal, const Float eta) noexcept
  {
    if (!isFinite(unitW_oLocal) || !isFinite(eta)) return std::nullopt;

    const Float cosineW_o{ clamp(unitW_oLocal[2], Float{ -1.0 }, Float{ 1.0 }) };
    const Float sineSqW_o{ clamp(differenceOfProducts(Float{ 1.0 }, Float{ 1.0 }, cosineW_o, cosineW_o), Float{}, Float{ 1.0 }) };
    const Float sineSqThetaT{ sqr(eta) * sineSqW_o };

    // TEST TIR
    if (sineSqThetaT >= Float{ 1.0 }) return std::nullopt;

    const Float cosineT{ safeSqrt(clamp(differenceOfProducts(Float{ 1.0 }, Float{ 1.0 }, sineSqThetaT, Float{ 1.0 }), Float{}, Float{ 1.0 })) };

    const Float signZ{ cosineW_o > Float{ 0.0 } ? Float{ -1.0 } : Float{ 1.0 } };

    Vec3f unitW_iLocal{ -eta * unitW_oLocal[0], -eta * unitW_oLocal[1], signZ * cosineT };
    unitW_iLocal = normalize(unitW_iLocal);

    if (!isFinite(unitW_iLocal)) return std::nullopt;

    return RefractResult{ unitW_iLocal, eta };
  }

  [[nodiscard]] Float trowbridgeReitz_D(const Vec3f& unitW_hLocal, Float alpha) noexcept 
  {
    if (!isFinite(unitW_hLocal)) return Float{};
    
    const Float cosTheta{ absCosineThetaLocal(unitW_hLocal) };
    
    if (cosTheta <= Float{}) return Float{};

    const Float a{ std::max(alpha, Float{ 1e-3f }) };
    const Float aSq{ a * a };
    const Float cosSq{ cosTheta * cosTheta };
    const Float tanSq{ (Float{ 1 } - cosSq) / cosSq };

    const Float denom{ kPi * cosSq * cosSq * sqr(aSq + tanSq) };

    if (denom <= Float{}) return Float{};

    return aSq / denom;
  }

  [[nodiscard]] Float microfacetAlphaFromRoughness(Float r) noexcept 
  {
    const Float rClamped{ clamp(r, Float{ 1e-3f }, Float{ 1 }) };
    return rClamped * rClamped;
  }

  [[nodiscard]] Float smithLambdaGGX(Float cosTheta, Float alpha) noexcept 
  {
    const Float absCosineTheta{ (cosTheta >= Float{} ? cosTheta : -cosTheta) };
    
    if (absCosineTheta <= Float{}) return Float{};

    const Float a{ (alpha < Float{ 1e-3f }) ? Float{ 1e-3f } : alpha };
    const Float cosSq{ absCosineTheta * absCosineTheta };
    const Float sinSq{ Float{ 1 } - cosSq };
    
    if (sinSq <= Float{}) return Float{};

    const Float tanSq{ sinSq / cosSq };
    const Float aSq{ a * a };
    const Float aSqTanSq{ aSq * tanSq };
    const Float root{ safeSqrt(Float{ 1 } + aSqTanSq) };

    return (root - Float{ 1 }) * Float { 0.5f };
  }

  [[nodiscard]] Float smithG1GGX(Float cosTheta, Float alpha) noexcept 
  {
    const Float lambda{ smithLambdaGGX(cosTheta, alpha) };

    return Float{ 1 } / (Float{ 1 } + lambda);
  }

  [[nodiscard]] Float smithGGX_G(const Vec3f& unitW_oLocal, const Vec3f& unitW_iLocal, Float alpha) noexcept 
  {
    const Float cosOut{ absCosineThetaLocal(unitW_oLocal) };
    const Float cosInc{ absCosineThetaLocal(unitW_iLocal) };

    return smithG1GGX(cosOut, alpha) * smithG1GGX(cosInc, alpha);
  }

  [[nodiscard]] Vec3f sampleGGXHalfVector(const Point2f& u, Float alpha) noexcept 
  {
    const Float a{ (alpha < Float{ 1e-3f }) ? Float{ 1e-3f } : alpha };
    const Float aSq{ a * a };

    const Float u1{ std::clamp(u[0], Float{ 1e-6f }, Float{ 1 } - Float{ 1e-6f }) };
    const Float u2{ u[1] };

    const Float tanSqTheta{ (aSq * u1) / (Float{ 1 } - u1) };
    const Float cosTheta{ Float{ 1 } / safeSqrt(Float{ 1 } + tanSqTheta) };
    const Float sinSqTheta{ std::max(Float{}, Float{ 1 } - cosTheta * cosTheta) };
    const Float sinTheta{ safeSqrt(sinSqTheta) };

    const Float phi{ Float{ 2 } * kPi * u2 };
    const Float cosPhi{ std::cos(phi) };
    const Float sinPhi{ std::sin(phi) };

    return Vec3f{ sinTheta * cosPhi, sinTheta * sinPhi, cosTheta };
  }
}