module;
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <psapi.h>

export module indus.stats.system_stats_collector;

import std;

import indus.core.types;

import indus.utilities.timing;

import indus.stats.accumulator;

export class SystemStatsCollector final
{
public:
  SystemStatsCollector() noexcept;
  ~SystemStatsCollector();

private:
  UInt64 cpuStart{};
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

  const Float64 cpuMicros{ static_cast<Float64>(cpuEnd - cpuStart) };
  const Float64 wallMicros{ static_cast<Float64>(std::chrono::duration_cast<std::chrono::microseconds>(wallEnd - wallStart).count()) };

  const Float64 cpuPct{ (wallMicros > Float64{ 0 }) ? (Float64{ 100.0 } * cpuMicros / wallMicros) : Float64{} };
  Float64 memMB{};

  PROCESS_MEMORY_COUNTERS pmc{};

  if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) 
  {
    memMB = static_cast<Float64>(pmc.WorkingSetSize) / (Float64(1024.0) * 1024.0);
  }

  StatsAccumulator::setMemoryMB(memMB);
  StatsAccumulator::setCPUUtilPct(cpuPct);
}
