export module core_diag;

// Set flag in the preprocessor to enable diag
#if INDUS_DIAG
  export constexpr bool diagCompileFlag{ true };
#else
  export constexpr bool diagCompileFlag{ false };
#endif

import <cstdint>;
import std;
import types;

export constexpr std::atomic<bool> diagRunFlag{ false };

export void diagCount(std::atomic<uint64_t>& count) noexcept
{
  if constexpr (diagCompileFlag)
  {
    count.fetch_add(1, std::memory_order_relaxed);
  }
}

export void diagCount(bool& b) noexcept
{
  if constexpr (diagCompileFlag)
  {
    b = true;
  }
}

export bool isFiniteVec(const Vec3f &vecToCheck) noexcept
{
  return std::isfinite(vecToCheck[0]) && std::isfinite(vecToCheck[1]) && std::isfinite(vecToCheck[2]);
}
