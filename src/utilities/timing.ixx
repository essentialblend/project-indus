module;
#define WIN32_LEAN_AND_MEAN

export module indus.utilities.timing;

import <Windows.h>;

import std;

import indus.core.types;

export
{
  std::string formatElapsedTime(Float64 seconds)
  {
    const auto total{ static_cast<Int64>(seconds) };

    const auto hours{ total / 3600 };
    const auto minutes{ (total % 3600) / 60 };
    const auto secs{ total % 60 };

    return std::format("{:02}:{:02}:{:02}", hours, minutes, secs);
  }

  UInt64 getProcessCPUTimeMicros() noexcept 
  {
    FILETIME creation{};
    FILETIME exit{};

    FILETIME kernel{};
    FILETIME user{};
    
    if (!::GetProcessTimes(::GetCurrentProcess(), &creation, &exit, &kernel, &user)) 
    {
      return 0;
    }

    ULARGE_INTEGER kernelFull{}, userFull{};
    
    kernelFull.LowPart = kernel.dwLowDateTime;
    kernelFull.HighPart = kernel.dwHighDateTime;
    userFull.LowPart = user.dwLowDateTime;
    userFull.HighPart = user.dwHighDateTime;

    // FILETIME is in 100-nanosecond units
    return (kernelFull.QuadPart + userFull.QuadPart) / 10;
  }
}