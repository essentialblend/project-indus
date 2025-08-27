export module core_diag;

// Set flag in the preprocessor to enable diag
#if INDUS_DIAG
  export constexpr bool diagCompileFlag{ true };
#else
  export constexpr bool diagCompileFlag{ false };
#endif

import <cstdint>;

import std;
import vec3;

export inline std::atomic<bool> diagRunFlag{ false };

export inline void diagCount(std::atomic<uint64_t>& count) noexcept
{
  if constexpr (diagCompileFlag)
  {
    count.fetch_add(1, std::memory_order_relaxed);
  }
}

export inline void diagCount(bool& b) noexcept
{
  if constexpr (diagCompileFlag)
  {
    b = true;
  }
}

export inline bool isFiniteVec(const Vec3 &vecToCheck) noexcept
{
  return std::isfinite(vecToCheck[0]) && std::isfinite(vecToCheck[1]) && std::isfinite(vecToCheck[2]);
}
