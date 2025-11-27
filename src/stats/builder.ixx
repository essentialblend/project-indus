export module indus.stats.builder;

import std;

import indus.core.types;

import indus.stats.constructs;

export class StatsBuilder final
{
public:
  static std::vector<SubSection> buildSubsections(const RenderStats& stats, bool isRenderComplete);

private:
  bool renderDone{};

  static SubSection buildIntegrator(const RenderStats& stats, bool isRenderComplete);
  static SubSection buildBVH(const RenderStats& stats, bool isRenderComplete);
  static SubSection buildIntersections(const RenderStats& stats, bool isRenderComplete);
  static SubSection buildMemory(const RenderStats& stats, bool isRenderComplete);
  static SubSection buildSystem(const RenderStats& stats, bool isRenderComplete);
};

static std::string formatNumber(std::uint64_t x, bool done)
{
  return (!done && x == 0) ? "--" : std::to_string(x);
}

std::vector<SubSection> StatsBuilder::buildSubsections(const RenderStats& stats, bool isRenderComplete)
{
  std::vector<SubSection> sections{};
  sections.reserve(5);

  sections.push_back(buildIntegrator(stats, isRenderComplete));
  sections.push_back(buildBVH(stats, isRenderComplete));
  sections.push_back(buildIntersections(stats, isRenderComplete));
  sections.push_back(buildMemory(stats, isRenderComplete));
  sections.push_back(buildSystem(stats, isRenderComplete));

  return sections;
}

SubSection StatsBuilder::buildIntegrator(const RenderStats& stats, bool isRenderComplete)
{
  SubSection section{};
  section.title = "Integrator";

  section.rows.push_back({ "Samples per pixel", formatNumber(stats.spp, isRenderComplete) });
  section.rows.push_back({ "Variance (avg)", isRenderComplete ? std::to_string(stats.varianceAverage) : "--" });
  section.rows.push_back({ "Clamped fireflies", formatNumber(stats.clampedFireflies, isRenderComplete) });

  section.rows.push_back({ "Camera rays", formatNumber(stats.raysCamera, isRenderComplete) });
  section.rows.push_back({ "Shadow rays", formatNumber(stats.raysShadow, isRenderComplete) });
  section.rows.push_back({ "Indirect rays", formatNumber(stats.raysIndirect, isRenderComplete) });
  section.rows.push_back({ "Total rays", formatNumber(stats.raysTotal, isRenderComplete) });

  return section;
}

SubSection StatsBuilder::buildBVH(const RenderStats& stats, bool isRenderComplete)
{
  SubSection section{};
  section.title = "BVH";

  section.rows.push_back({ "Interior nodes", formatNumber(stats.BVHInteriorNodes, isRenderComplete) });
  section.rows.push_back({ "Leaf nodes", formatNumber(stats.BVHLeafNodes, isRenderComplete) });

  if (!isRenderComplete || stats.nodesVisited.count == 0)
  {
    section.rows.push_back({ "Nodes visited / ray", "--" });
    return section;
  }

  const Float64 avg{ Float64(stats.nodesVisited.sum) / Float64(stats.nodesVisited.count) };

  std::string value
  {
    "avg " + std::to_string(avg) +
    " (min " + std::to_string(stats.nodesVisited.min) +
    ", max " + std::to_string(stats.nodesVisited.max) + ")"
  };

  section.rows.push_back({ "Nodes visited / ray", std::move(value) });
  return section;
}

SubSection StatsBuilder::buildIntersections(const RenderStats& stats, bool isRenderComplete)
{
  SubSection section{};
  section.title = "Intersections";

  section.rows.push_back({ "Regular intersection tests", formatNumber(stats.regularIntersectionTests, isRenderComplete) });

  section.rows.push_back({ "Ray-primitive tests", formatNumber(stats.rayPrimitiveTests, isRenderComplete) });

  if (!isRenderComplete || !stats.hitPercent.isDefined())
  {
    section.rows.push_back({ "Hit percent", "--" });
    return section;
  }

  const Float64 pct{ stats.hitPercent.valueOr(Float64{ 0.0 }) * Float64{ 100.0 } };
  const Int rounded{ static_cast<Int>(pct + 0.5) };

  section.rows.push_back({ "Hit percent", std::to_string(rounded) + "%" });
  
  return section;
}

SubSection StatsBuilder::buildMemory(const RenderStats& stats, bool isRenderComplete)
{
  SubSection section{};
  section.title = "Memory";

  auto mbString{ [isRenderComplete](std::uint64_t bytes)
  {
    if (!isRenderComplete && bytes == 0) return std::string("--");
    if (bytes == 0) return std::string("0.000 MB");

    const Float64 mb{ static_cast<Float64>(bytes) / (Float64(1024.0) * 1024.0) };

    return std::format("{:.3f} MB", mb);
  }};

  section.rows.push_back({ "BVH", mbString(stats.bytesBVH) });
  section.rows.push_back({ "Geometry", mbString(stats.bytesGeometry) });
  section.rows.push_back({ "Textures", mbString(stats.bytesTextures) });
  section.rows.push_back({ "Film", mbString(stats.bytesFilm) });

  return section;
}

SubSection StatsBuilder::buildSystem(const RenderStats& stats, bool isRenderComplete)
{
  SubSection section{};
  section.title = "System";

  if (!isRenderComplete)
  {
    section.rows.push_back({ "CPU util", "--" });
    section.rows.push_back({ "Process memory", "--" });
    return section;
  }

  const int cpu{ static_cast<int>(stats.CPUUtilPct + 0.5) };
  const int mem{ static_cast<int>(stats.memoryMB + 0.5) };

  section.rows.push_back({ "CPU util", std::to_string(cpu) + "%" });
  section.rows.push_back({ "Process memory", std::to_string(mem) + " MB" });

  return section;
}

