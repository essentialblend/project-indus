export module renderprogress;

import std;
import types;
import bounds;

export class RenderProgress final 
{
public:
  explicit RenderProgress(Int totalTiles, Int barWidth = 40) noexcept;

  void begin() noexcept;
  void tileDone() noexcept;
  void done() noexcept;

  float getNormalizedProgress() const noexcept;

private:
  Int m_total{};
  std::atomic<Int> m_done{ 0 };
  std::atomic<Int> m_lastPct{ -1 };
  Int m_barWidth{ 40 };
  Int m_lastLen{};

  void printLine(Int pct) noexcept;
};

RenderProgress::RenderProgress(Int totalTiles, Int barWidth) noexcept : m_total{ totalTiles }, m_barWidth{ barWidth } {}

void RenderProgress::begin() noexcept 
{
  m_done.store(0, std::memory_order_relaxed);
  
  m_lastPct.store(-1, std::memory_order_relaxed);
  
  printLine(0);
}

void RenderProgress::tileDone() noexcept 
{
  const Int doneNow{ m_done.fetch_add(1, std::memory_order_relaxed) + 1 };

  const Int pct{ static_cast<Int>((100LL * doneNow) / m_total) };
  
  Int expected{ m_lastPct.load(std::memory_order_relaxed) };
  
  if (pct > expected && m_lastPct.compare_exchange_strong(expected, pct, std::memory_order_relaxed)) 
  {
    printLine(pct);
  }
}

void RenderProgress::done() noexcept
{
  m_lastPct.store(100, std::memory_order_relaxed);

  printLine(100);
  
  std::println();
}

float RenderProgress::getNormalizedProgress() const noexcept
{
  return static_cast<float>(m_done) / static_cast<float>(m_total);
}

void RenderProgress::printLine(Int pct) noexcept 
{
  m_lastPct = pct;

  const Int filled{ (m_barWidth * pct) / 100 };

  std::string bar(filled, '*');
  bar.append(m_barWidth - filled, '-');

  std::string line{ std::format("Render: [{}] {:3d}%", bar, pct) };

  std::print("\r{}{}", line, std::string(std::max(0, m_lastLen - static_cast<Int>(line.size())), ' '));

  std::fflush(nullptr);

  m_lastLen = static_cast<Int>(line.size());
}
