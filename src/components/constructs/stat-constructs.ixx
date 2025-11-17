export module statconstructs;

import std;

export
{
  struct Ratio64
  {
    std::uint64_t numerator{};
    std::uint64_t denominator{};

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
    std::uint64_t count{};
    std::uint64_t sum{};
    std::uint64_t min{ std::numeric_limits<std::uint64_t>::max() };
    std::uint64_t max{ std::numeric_limits<std::uint64_t>::min() };
  };

  struct RenderStats
  {
    // Integrator / sampling
    std::uint64_t spp{};
    double varianceAverage{};
    std::uint64_t clampedFireflies{};

    // Rays
    std::uint64_t raysCamera{};
    std::uint64_t raysShadow{};
    std::uint64_t raysIndirect{};
    std::uint64_t raysTotal{};

    // BVH & traversal quality
    Ratio64 hitPercent{};
    IntDistribution nodesVisited{};
    std::uint64_t BVHInteriorNodes{};
    std::uint64_t BVHLeafNodes{};

    // Intersections
    std::uint64_t regularIntersectionTests{};
    std::uint64_t rayPrimitiveTests{};

    // Memory
    std::uint64_t bytesBVH{};
    std::uint64_t bytesGeometry{};
    std::uint64_t bytesTextures{};
    std::uint64_t bytesFilm{};

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