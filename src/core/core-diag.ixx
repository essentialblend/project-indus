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
import concepts;

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

export template<Arithmetic T>
constexpr bool isFinite(T toCheck) noexcept
{
  return std::isfinite(toCheck);
}

export template<VectorLike V>
constexpr bool isFinite(const V& v) noexcept
{
  return (isFinite(v[0]) && isFinite(v[1]) && isFinite(v[2]));
}