export module indus.stats.constructs;

import std;

import indus.core.types;

export
{
  struct Ratio64
  {
    UInt64 numerator{};
    UInt64 denominator{};

    bool isDefined() const noexcept
    {
      return denominator != 0;
    }

    std::optional<double> tryValue() const noexcept 
    {
      return isDefined() ? std::optional<double>(double(numerator) / double(denominator)) : std::nullopt;
    }

    double valueOr(double fallback) const noexcept 
    {
      return isDefined() ? double(numerator) / double(denominator) : fallback;
    }
  };

  struct IntDistribution
  {
    UInt64 count{};
    UInt64 sum{};
    UInt64 min{ std::numeric_limits<UInt64>::max() };
    UInt64 max{ std::numeric_limits<UInt64>::min() };
  };

  struct RenderStats
  {
    // Integrator / sampling
    UInt64 spp{};
    double varianceAverage{};
    UInt64 clampedFireflies{};

    // Rays
    UInt64 raysCamera{};
    UInt64 raysShadow{};
    UInt64 raysIndirect{};
    UInt64 raysTotal{};

    // BVH & traversal quality
    Ratio64 hitPercent{};
    IntDistribution nodesVisited{};
    UInt64 BVHInteriorNodes{};
    UInt64 BVHLeafNodes{};

    // Intersections
    UInt64 regularIntersectionTests{};
    UInt64 rayPrimitiveTests{};

    // Memory
    UInt64 bytesBVH{};
    UInt64 bytesGeometry{};
    UInt64 bytesTextures{};
    UInt64 bytesFilm{};

    // System
    double CPUUtilPct{};
    double memoryMB{};
  };

  struct KeyValueRow
  {
    std::string label;
    std::string value;
  };

  struct SubSection
  {
    std::string title;
    std::vector<KeyValueRow> rows;
  };

  struct StatsViewportGeometry
  {
    float left{};
    float top{};
    float width{};
    float height{};
    float yBaseCenter{};
  };
};