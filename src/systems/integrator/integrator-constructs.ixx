export module indus.integrator.constructs;

import std;

import indus.film.image;

import indus.stats.constructs;

export
{
  struct FrameSnapshot final
  {
    Image image{};
    Float progressUnitNormalized{};
    UInt64 frameVersion{};
    std::optional<RenderStats> renderStats{};
  };

  using DisplayConsumer = std::function<void(FrameSnapshot)>;
};