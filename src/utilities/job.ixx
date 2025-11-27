export module indus.utilities.job;

import std;

import indus.core.types;
import indus.core.geom.bounds;

export class JobBase
{
public:
  virtual ~JobBase() = default;

  virtual bool hasWork() const noexcept = 0;
  virtual void runStep() noexcept = 0;
};

export class ParallelJob1D final : public JobBase
{
public:
  using Functor = std::function<void(Int64, Int64)>;

  ParallelJob1D(Int64 begin, Int64 end, Int64 chunkSize, Functor fn) noexcept;

  bool hasWork() const noexcept override;
  void runStep() noexcept override;

private:
  Int64 m_end{};
  Int64 m_chunk{};

  Functor m_fn;
  
  std::atomic<Int64> m_next{};
};

ParallelJob1D::ParallelJob1D(Int64 begin, Int64 end, Int64 chunkSize, Functor fn) noexcept : m_end{ end }, m_chunk{ std::max<Int64>(1, chunkSize) }, m_fn{ std::move(fn) }, m_next{ begin } {}

bool ParallelJob1D::hasWork() const noexcept
{
  return m_next.load(std::memory_order_relaxed) < m_end;
}

void ParallelJob1D::runStep() noexcept
{
  const auto start{ m_next.fetch_add(m_chunk, std::memory_order_relaxed) };

  if (start >= m_end) return;

  const auto stop{ std::min<Int64>(start + m_chunk, m_end) };

  m_fn(start, stop);
}

export class ParallelJob2D final : public JobBase 
{
public:
  using Functor = std::function<void(const Bounds2i&)>;

  explicit ParallelJob2D(std::vector<Bounds2i> tiles, Functor fn) noexcept;

  bool hasWork() const noexcept override;

  void runStep() noexcept override;

private:
  std::vector<Bounds2i> m_tiles;
  Functor m_fn;
  std::atomic_size_t m_next{ 0 };
};

ParallelJob2D::ParallelJob2D(std::vector<Bounds2i> tiles, Functor fn) noexcept : m_tiles(std::move(tiles)), m_fn(std::move(fn)) {}

bool ParallelJob2D::hasWork() const noexcept 
{
  return m_next.load(std::memory_order_relaxed) < m_tiles.size();
}

void ParallelJob2D::runStep() noexcept
{
  const auto i{ m_next.fetch_add(1, std::memory_order_relaxed) };

  if (i >= m_tiles.size()) return;

  m_fn(m_tiles[i]);
}
