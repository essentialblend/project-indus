export module scanlines;

import std;
import types;

export class ScanlineProgress final
{
public:
  explicit ScanlineProgress(Int totalRows, Int barWidth) noexcept;

  void begin() noexcept;
  constexpr void lineDone(Int y) noexcept;
  void done() noexcept;

private:
  Int m_total{};
  Int m_done{};
  Int m_lastPct{ -1 };
  Int m_barWidth{ 40 };
  Int m_lastLen{};

  void printLine(Int pct) noexcept;
};

ScanlineProgress::ScanlineProgress(Int totalRows, Int barWidth) noexcept : m_total{ totalRows }, m_done{}, m_lastPct{ -1 }, m_barWidth{ barWidth } {}

void ScanlineProgress::begin() noexcept
{
  m_done = 0;
  m_lastPct = -1;

  printLine(0);
}

constexpr void ScanlineProgress::lineDone(Int) noexcept
{
  ++m_done;

  const Int pct{ static_cast<Int>((100LL * m_done) / m_total) };
  
  if (pct != m_lastPct) printLine(pct);
}

void ScanlineProgress::done() noexcept
{
  printLine(100);
  std::println();
}

void ScanlineProgress::printLine(Int pct) noexcept
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