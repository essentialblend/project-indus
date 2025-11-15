export module parallelutil;

import std;
import types;
import bounds;
import threadpool;

export namespace ParallelSystems
{
  namespace Variables
  {
    std::optional<std::reference_wrapper<ThreadPool>> g_engineThreadPoolOpt{};
    std::once_flag g_onceFlag{};
  }

  void setEngineThreadPool(ThreadPool& threadPool) noexcept
  {
    std::call_once(Variables::g_onceFlag, [&] { Variables::g_engineThreadPoolOpt.emplace(threadPool); });
  }

  ThreadPool& getEngineThreadPool() noexcept
  {
    return Variables::g_engineThreadPoolOpt->get();
  }

  bool hasEngineThreadPool() noexcept
  {
    return Variables::g_engineThreadPoolOpt.has_value();
  }

  void resetThreadPool() noexcept
  {
    Variables::g_engineThreadPoolOpt.reset();
  }
}

export Int parallelTileCount(const Bounds2i& extent) noexcept
{
  if (extent.isEmpty()) return 0;

  const auto& lo{ extent.getMin() };
  const auto& hi{ extent.getMax() };

  const Int W{ hi[0] - lo[0] };
  const Int H{ hi[1] - lo[1] };

  const auto& pool{ ParallelSystems::getEngineThreadPool() };

  const std::size_t R{ std::max<std::size_t>(1, pool.getSize() + 1) };

  const Int t{ std::clamp<Int>(static_cast<Int>(std::sqrt(double(W) * double(H) / (8.0 * double(R)))), 1, 32) };

  return ((W + t - 1) / t) * ((H + t - 1) / t);
}

export template<class F>
void parallelFor2D(const Bounds2i& extent, F&& functor)
{
  if (extent.isEmpty()) return;

  auto& pool{ ParallelSystems::getEngineThreadPool() };

  const auto& minExtentBounds{ extent.getMin() };
  const auto& maxExtentBounds{ extent.getMax() };
  
  const std::size_t threads{ std::max<std::size_t>(1, pool.getSize() + 1) };

  const auto spanX{ static_cast<double>(maxExtentBounds[0] - minExtentBounds[0]) };
  const auto spanY{ static_cast<double>(maxExtentBounds[1] - minExtentBounds[1]) };

  // Decide tile size based on number of threads and image extent. That is, we want to figure N such that we have N * N tiles and N * N >= 8 * threads. Then, poolDimensionalSpan = N = sqrt(A / 8P) where P is number of threads and A is area of the extent
  const auto poolDimensionalSpan{ std::sqrt(spanX * spanY / (8.0 * static_cast<double>(threads))) };

  const Int t{ std::clamp(static_cast<Int>(poolDimensionalSpan), 1, 32) };
  
  std::vector<Bounds2i> tiles{};
  tiles.reserve(parallelTileCount(extent));
  
  for (Int y{ minExtentBounds[1] }; y < maxExtentBounds[1]; y += t)
  {
    for (Int x{ minExtentBounds[0] }; x < maxExtentBounds[0]; x += t)
    {
      tiles.emplace_back(Point2i{ x, y }, Point2i{ std::min<Int>(x + t, maxExtentBounds[0]), std::min<Int>(y + t,maxExtentBounds[1]) });
    }
  }
    
  auto job{ std::make_shared<ParallelJob2D>(std::move(tiles), std::function<void(const Bounds2i&)>(std::forward<F>(functor))) };

  pool.enqueueJob(job);
  
  pool.drainThreadPool();
}


