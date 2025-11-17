export module threadlocalstorage;

import std;
import statconstructs;
import miscconstructs;

export class ThreadLocalStorage final
{
public:
  ThreadLocalStorage() = default;

  void setThreadID(std::size_t ID) noexcept;
  std::size_t getThreadID() const noexcept;

  template<class Self>
  decltype(auto) getRenderStats(this Self&& self) noexcept;

  void setRenderStats(const RenderStats& renderStats) noexcept;
  void resetRenderStats() noexcept;

  // Rays
  void incrementCameraRays(bool isDirectRay = false) noexcept;

  // BVH
  void accumulateNodesVisitedBVH(std::uint64_t numNodes) noexcept;
  void incrementPrimitiveTestsBVH(std::uint64_t n = 1) noexcept;
  void incrementHitRaysBVH() noexcept;
  void incrementTotalRaysTestedBVH() noexcept;
  void computeBVHCounts(const std::vector<LinearBVHNode>& nodes) noexcept;
  void incrementAABBTests() noexcept;
  void setBVHBytes(std::uint64_t bytes) noexcept;

  // Memory
  void setGeometryBytes(std::uint64_t b) noexcept;
  void setTextureBytes(std::uint64_t b) noexcept;
  void setFilmBytes(std::uint64_t b) noexcept;
  void addGeometryBytes(std::uint64_t b) noexcept;

private:
  std::size_t m_threadID{};
  RenderStats m_renderStats{};
};

void ThreadLocalStorage::setThreadID(std::size_t ID) noexcept
{
  m_threadID = ID;
}

std::size_t ThreadLocalStorage::getThreadID() const noexcept
{
  return m_threadID;
}

template<class Self>
decltype(auto) ThreadLocalStorage::getRenderStats(this Self&& self) noexcept
{
  return std::forward_like<Self>(self.m_renderStats);
}

void ThreadLocalStorage::setRenderStats(const RenderStats& renderStats) noexcept
{
  m_renderStats = renderStats;
}

void ThreadLocalStorage::incrementCameraRays(bool isDirectRay) noexcept
{
  isDirectRay ? ++m_renderStats.raysCamera : ++m_renderStats.raysIndirect;
  ++m_renderStats.raysTotal;
}

void ThreadLocalStorage::accumulateNodesVisitedBVH(std::uint64_t numNodes) noexcept
{
  auto& dst{ m_renderStats.nodesVisited };

  dst.count++;
  dst.sum += numNodes;

  if (dst.min > numNodes) dst.min = numNodes;
  if (dst.max < numNodes) dst.max = numNodes;
}

void ThreadLocalStorage::incrementPrimitiveTestsBVH(std::uint64_t n) noexcept
{
  m_renderStats.rayPrimitiveTests += n;
}

void ThreadLocalStorage::incrementHitRaysBVH() noexcept
{
  ++m_renderStats.hitPercent.numerator;
}

void ThreadLocalStorage::incrementTotalRaysTestedBVH() noexcept
{
  ++m_renderStats.hitPercent.denominator;
}

void ThreadLocalStorage::computeBVHCounts(const std::vector<LinearBVHNode>& nodes) noexcept
{
  std::uint64_t interior{};
  std::uint64_t leaf{};

  for (const auto& ln : nodes)
  {
    if (ln.primitiveCount > 0) ++leaf;
    else ++interior;
  }

  m_renderStats.BVHInteriorNodes = interior;
  m_renderStats.BVHLeafNodes = leaf;
}

void ThreadLocalStorage::incrementAABBTests() noexcept
{
  ++m_renderStats.regularIntersectionTests;
}

void ThreadLocalStorage::setBVHBytes(std::uint64_t bytes) noexcept
{
  m_renderStats.bytesBVH = bytes;
}

void ThreadLocalStorage::setGeometryBytes(std::uint64_t b) noexcept
{
  m_renderStats.bytesGeometry = b;
}

void ThreadLocalStorage::addGeometryBytes(std::uint64_t b) noexcept
{
  m_renderStats.bytesGeometry += b;
}

void ThreadLocalStorage::setTextureBytes(std::uint64_t b) noexcept
{
  m_renderStats.bytesTextures = b;
}

void ThreadLocalStorage::setFilmBytes(std::uint64_t b) noexcept
{
  m_renderStats.bytesFilm = b;
}

void ThreadLocalStorage::resetRenderStats() noexcept
{
  m_renderStats = RenderStats{};
}