export module basictimer;

import std;
import types;

export class BasicTimer
{
public:
  BasicTimer() noexcept = default;

  void startTimer() noexcept;
  void stopTimer() noexcept;

  Int64 getMillisec() const noexcept;
  Float64 getSeconds() const noexcept;

private:
  std::chrono::steady_clock::time_point m_startTime{};
  std::chrono::steady_clock::time_point m_endTime{};
  bool m_stopped{ false };
};

void BasicTimer::startTimer() noexcept
{
  if (!m_stopped)
    m_startTime = std::chrono::steady_clock::now();
}

void BasicTimer::stopTimer() noexcept
{ 
  if (!m_stopped) 
  { 
    m_endTime = std::chrono::steady_clock::now(); 
    m_stopped = true; 
  } 
}

Int64 BasicTimer::getMillisec() const noexcept
{
  const auto t{ m_stopped ? m_endTime : std::chrono::steady_clock::now() };

  return std::chrono::duration_cast<std::chrono::milliseconds>(t - m_startTime).count();
}

Float64 BasicTimer::getSeconds() const noexcept
{ 
  return getMillisec() / Float64{ 1000 };
}
