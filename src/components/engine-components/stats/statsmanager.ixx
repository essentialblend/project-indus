export module statsmanager;

import std;
import statconstructs;
import parallelutil;
import threadutil;

export class StatsManager final
{
public:
  void beginFrame() noexcept;
  RenderStats endFrame() noexcept;

private:
  std::vector<RenderStats> m_perThread{};

  static void merge(RenderStats& dst, const RenderStats& src) noexcept;
};

void StatsManager::beginFrame() noexcept
{
  ParallelSystems::getEngineThreadPool().forEachThread([] {
    TLSObj.resetRenderStats();
    });
}

RenderStats StatsManager::endFrame() noexcept
{
  const std::size_t nThreads =
    std::max<std::size_t>(std::size_t{ 1 },
      ParallelSystems::getEngineThreadPool().getSize());

  m_perThread.assign(nThreads, RenderStats{});

  ParallelSystems::getEngineThreadPool().forEachThread([&] {
    const std::size_t id = TLSObj.getThreadID();
    if (id < m_perThread.size())
      m_perThread[id] = TLSObj.getRenderStats();
    });

  RenderStats out{};
  for (const auto& s : m_perThread)
    merge(out, s);

  const double denom = static_cast<double>(nThreads);
  out.CPUUtilPct /= denom;
  out.memoryMB /= denom;

  return out;
}

void StatsManager::merge(RenderStats& dst, const RenderStats& src) noexcept
{
  // Integrator
  dst.spp += src.spp;
  dst.varianceAverage += src.varianceAverage;
  dst.clampedFireflies += src.clampedFireflies;

  // Rays
  dst.raysCamera += src.raysCamera;
  dst.raysShadow += src.raysShadow;
  dst.raysIndirect += src.raysIndirect;
  dst.raysTotal += src.raysTotal;

  // BVH
  dst.BVHInteriorNodes += src.BVHInteriorNodes;
  dst.BVHLeafNodes += src.BVHLeafNodes;

  // IntDistribution
  dst.nodesVisited.count += src.nodesVisited.count;
  dst.nodesVisited.sum += src.nodesVisited.sum;
  dst.nodesVisited.min = std::min(dst.nodesVisited.min, src.nodesVisited.min);
  dst.nodesVisited.max = std::max(dst.nodesVisited.max, src.nodesVisited.max);

  // Intersections
  dst.regularIntersectionTests += src.regularIntersectionTests;
  dst.rayPrimitiveTests += src.rayPrimitiveTests;

  // Memory
  dst.bytesBVH += src.bytesBVH;
  dst.bytesGeometry += src.bytesGeometry;
  dst.bytesTextures += src.bytesTextures;
  dst.bytesFilm += src.bytesFilm;

  // System (sum; divide by thread-count after merge)
  dst.CPUUtilPct += src.CPUUtilPct;
  dst.memoryMB += src.memoryMB;
}
