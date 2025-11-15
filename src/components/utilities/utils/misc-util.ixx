export module miscutil;

import std;
import types;

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
}