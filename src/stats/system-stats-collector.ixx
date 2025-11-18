module;
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <psapi.h>

export module indus.stats.system_stats_collector;

import std;

import indus.utilities.timing;

import indus.stats.accumulator;

export class SystemStatsCollector final
{
public:
  SystemStatsCollector() noexcept;
  ~SystemStatsCollector();

private:
  std::uint64_t cpuStart{};
  std::chrono::steady_clock::time_point wallStart{};
};

SystemStatsCollector::SystemStatsCollector() noexcept
{
  cpuStart = getProcessCPUTimeMicros();
  wallStart = std::chrono::steady_clock::now();
}

SystemStatsCollector::~SystemStatsCollector()
{
  const auto cpuEnd{ getProcessCPUTimeMicros() };
  const auto wallEnd{ std::chrono::steady_clock::now() };

  const double cpuMicros{ static_cast<double>(cpuEnd - cpuStart) };
  const double wallMicros{ static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(wallEnd - wallStart).count()) };

  const double cpuPct{ (wallMicros > 0) ? (100.0 * cpuMicros / wallMicros) : 0.0 };
  double memMB{};

  PROCESS_MEMORY_COUNTERS pmc{};

  if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) 
  {
    memMB = static_cast<double>(pmc.WorkingSetSize) / (1024.0 * 1024.0);
  }

  StatsAccumulator::setMemoryMB(memMB);
  StatsAccumulator::setCPUUtilPct(cpuPct);
}
