export module indus.stats.accumulator;

import std;

import indus.core.types;

import indus.stats.constructs;

export class StatsAccumulator final
{
public:
  static void reset(std::size_t maxThreads) noexcept;
  [[nodiscard]] static RenderStats finalize() noexcept;

  static void setBVHNodeCounts(std::uint64_t interior, std::uint64_t leaf) noexcept;
  static void setBVHBytes(std::uint64_t bytes) noexcept;
  static void setCPUUtilPct(Float64 pct) noexcept;
  static void setMemoryMB(Float64 mb) noexcept;
  static void setFilmBytes(std::uint64_t bytes) noexcept;

  static void addGeometryBytes(std::uint64_t bytes) noexcept;
  static void addTextureBytes(std::uint64_t bytes) noexcept;

  static void recordCameraRay() noexcept;
  static void recordIndirectRay() noexcept;

  static void recordBVHNodesVisited(std::uint64_t nodesVisited) noexcept;
  static void recordRayPrimitiveTests(std::uint64_t nTests) noexcept;
  static void recordBVHHit(bool hit) noexcept;
  static void recordRegularIntersectionTest() noexcept;

private:
  inline static std::vector<RenderStats> s_counters{};
  inline static std::atomic<std::size_t> s_nextSlot{ 0 };
  inline static thread_local std::size_t s_threadIndex{ std::numeric_limits<std::size_t>::max() };

  inline static std::atomic<std::uint64_t> s_bvhInteriorNodes{ 0 };
  inline static std::atomic<std::uint64_t> s_bvhLeafNodes{ 0 };
  inline static std::atomic<std::uint64_t> s_bvhBytes{ 0 };

  inline static Float64 s_cpuUtilPct{};
  inline static Float64 s_memoryMB{};

  inline static std::atomic<std::uint64_t> s_filmBytes{ 0 };
  inline static std::atomic<std::uint64_t> s_geometryBytes{ 0 };
  inline static std::atomic<std::uint64_t> s_textureBytes{ 0 };

  [[nodiscard]] static RenderStats& getMutableCurrentCounter() noexcept;
  [[nodiscard]] static const std::vector<RenderStats>& getAllCounters() noexcept;
};

void StatsAccumulator::reset(std::size_t maxThreads) noexcept
{
  if (maxThreads == 0) maxThreads = 1;
  if (maxThreads > s_counters.size()) s_counters.resize(maxThreads);

  for (auto& c : s_counters) c = RenderStats{};

  s_bvhInteriorNodes.store(0, std::memory_order_relaxed);
  s_bvhLeafNodes.store(0, std::memory_order_relaxed);
  s_bvhBytes.store(0, std::memory_order_relaxed);
  s_filmBytes.store(0, std::memory_order_relaxed);
  s_geometryBytes.store(0, std::memory_order_relaxed);
  s_textureBytes.store(0, std::memory_order_relaxed);

  s_cpuUtilPct = Float64{};
  s_memoryMB = Float64{};
}

RenderStats StatsAccumulator::finalize() noexcept
{
  RenderStats total{};

  for (const auto& c : getAllCounters())
  {
    total.spp += c.spp;
    total.raysCamera += c.raysCamera;
    total.raysShadow += c.raysShadow;
    total.raysIndirect += c.raysIndirect;
    total.raysTotal += c.raysTotal;

    total.regularIntersectionTests += c.regularIntersectionTests;
    total.rayPrimitiveTests += c.rayPrimitiveTests;

    total.nodesVisited.count += c.nodesVisited.count;
    total.nodesVisited.sum += c.nodesVisited.sum;
    
    if (c.nodesVisited.count > 0)
    {
      total.nodesVisited.min = std::min(total.nodesVisited.min, c.nodesVisited.min);
      total.nodesVisited.max = std::max(total.nodesVisited.max, c.nodesVisited.max);
    }

    total.hitPercent.numerator += c.hitPercent.numerator;
    total.hitPercent.denominator += c.hitPercent.denominator;
  }

  total.BVHInteriorNodes = s_bvhInteriorNodes.load(std::memory_order_relaxed);
  total.BVHLeafNodes = s_bvhLeafNodes.load(std::memory_order_relaxed);

  total.bytesBVH = s_bvhBytes.load(std::memory_order_relaxed);
  total.bytesFilm = s_filmBytes.load(std::memory_order_relaxed);
  total.bytesGeometry = s_geometryBytes.load(std::memory_order_relaxed);
  total.bytesTextures = s_textureBytes.load(std::memory_order_relaxed);

  total.CPUUtilPct = s_cpuUtilPct;
  total.memoryMB = s_memoryMB;

  return total;
}

void StatsAccumulator::recordCameraRay() noexcept
{
  RenderStats& s{ getMutableCurrentCounter() };
  ++s.raysCamera;
  ++s.raysTotal;
}

void StatsAccumulator::recordIndirectRay() noexcept
{
  RenderStats& s{ getMutableCurrentCounter() };
  ++s.raysIndirect;
  ++s.raysTotal;
}

RenderStats& StatsAccumulator::getMutableCurrentCounter() noexcept
{
  constexpr std::size_t invalid{ std::numeric_limits<std::size_t>::max() };
  std::size_t index{ s_threadIndex };

  if (index == invalid)
  {
    index = s_nextSlot.fetch_add(1, std::memory_order_relaxed);

    if (s_counters.empty())
    {
      s_counters.resize(1);
      index = 0;
    }
    else if (index >= s_counters.size())
    {
      index = s_counters.size() - 1;
    }

    s_threadIndex = index;
  }

  return s_counters[index];
}

const std::vector<RenderStats>& StatsAccumulator::getAllCounters() noexcept
{
  return s_counters;
}

void StatsAccumulator::recordBVHNodesVisited(std::uint64_t nodesVisited) noexcept
{
  RenderStats& s{ getMutableCurrentCounter() };
  IntDistribution& d{ s.nodesVisited };

  ++d.count;
  d.sum += nodesVisited;

  if (nodesVisited < d.min){ d.min = nodesVisited; }
  if (nodesVisited > d.max){ d.max = nodesVisited; }
}

void StatsAccumulator::recordRayPrimitiveTests(std::uint64_t nTests) noexcept
{
  RenderStats& s{ getMutableCurrentCounter() };
  s.rayPrimitiveTests += nTests;
}

void StatsAccumulator::recordBVHHit(bool hit) noexcept
{
  RenderStats& s{ getMutableCurrentCounter() };
  ++s.hitPercent.denominator;
  if (hit) ++s.hitPercent.numerator;
}

void StatsAccumulator::recordRegularIntersectionTest() noexcept
{
  RenderStats& s{ getMutableCurrentCounter() };
  ++s.regularIntersectionTests;
}

void StatsAccumulator::setBVHNodeCounts(std::uint64_t interior, std::uint64_t leaf) noexcept
{
  s_bvhInteriorNodes.store(interior, std::memory_order_relaxed);
  s_bvhLeafNodes.store(leaf, std::memory_order_relaxed);
}

void StatsAccumulator::setBVHBytes(std::uint64_t bytes) noexcept
{
  s_bvhBytes.store(bytes, std::memory_order_relaxed);
}

void StatsAccumulator::setCPUUtilPct(Float64 pct) noexcept 
{ 
  s_cpuUtilPct = pct; 
}

void StatsAccumulator::setMemoryMB(Float64 mb) noexcept 
{ 
  s_memoryMB = mb; 
}

void StatsAccumulator::setFilmBytes(std::uint64_t bytes) noexcept
{
  s_filmBytes.store(bytes, std::memory_order_relaxed);
}

void StatsAccumulator::addGeometryBytes(std::uint64_t bytes) noexcept
{
  s_geometryBytes.fetch_add(bytes, std::memory_order_relaxed);
}

void StatsAccumulator::addTextureBytes(std::uint64_t bytes) noexcept
{
  s_textureBytes.fetch_add(bytes, std::memory_order_relaxed);
}