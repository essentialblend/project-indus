export module threadlocalstorage;

import std;
import statconstructs;

export class ThreadLocalStorage final
{
public:
  ThreadLocalStorage() = default;

  void setThreadID(std::size_t ID) noexcept;
  std::size_t getThreadID() const noexcept;

  template<class Self>
  decltype(auto) getRenderStats(this Self&& self) noexcept;

  void setRenderStats(const RenderStats& renderStats) noexcept;

private:
  std::size_t m_threadID{};
  RenderStats m_renderStats{};
};

void ThreadLocalStorage::setThreadID(std::size_t ID) noexcept
{
  m_threadID = ID;
}

std::size_t ThreadLocalStorage::getThreadID() const noexcept
{
  return m_threadID;
}

template<class Self>
decltype(auto) ThreadLocalStorage::getRenderStats(this Self&& self) noexcept
{
  return std::forward_like<Self>(self.m_renderStats);
}

void ThreadLocalStorage::setRenderStats(const RenderStats& renderStats) noexcept
{
  m_renderStats = renderStats;
}


