export module framemailbox;

import std;
import engineconstructs;

export class FrameMailbox final
{
public:
  FrameMailbox() noexcept = default;

  void publishFrameSnapshot(FrameSnapshot frameSnapshot) noexcept;
  std::optional<FrameSnapshot> tryConsume() noexcept;

private:
  FrameSnapshot m_frameSnapshotBuffer[2]{};
  std::atomic<std::uint64_t> version{};
};

void FrameMailbox::publishFrameSnapshot(FrameSnapshot frameSnapshot) noexcept
{
  const std::uint64_t next{ version.load(std::memory_order_relaxed) + 1 };
  const std::size_t idx{ static_cast<std::size_t>(next & 1u) };

  m_frameSnapshotBuffer[idx] = std::move(frameSnapshot);
  
  version.store(next, std::memory_order_release);
}

std::optional<FrameSnapshot> FrameMailbox::tryConsume() noexcept
{
  const std::uint64_t k{ version.load(std::memory_order_acquire) };

  if (k == 0) return std::nullopt;
  
  const std::size_t idx{ static_cast<std::size_t>(k & 1u) };

  return m_frameSnapshotBuffer[idx];
}
