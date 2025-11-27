export module indus.engine.frame_mailbox;

import std;

import indus.integrator.constructs;

export class FrameMailbox final
{
public:
  FrameMailbox() noexcept = default;

  void publishFrameSnapshot(FrameSnapshot frameSnapshot) noexcept;
  std::optional<FrameSnapshot> tryConsume() noexcept;

private:
  FrameSnapshot m_frameSnapshotBuffer[2]{};
  std::atomic<UInt64> version{};
};

void FrameMailbox::publishFrameSnapshot(FrameSnapshot frameSnapshot) noexcept
{
  const UInt64 next{ version.load(std::memory_order_relaxed) + 1 };
  const std::size_t idx{ static_cast<std::size_t>(next & 1u) };

  m_frameSnapshotBuffer[idx] = std::move(frameSnapshot);
  
  version.store(next, std::memory_order_release);
}

std::optional<FrameSnapshot> FrameMailbox::tryConsume() noexcept
{
  const UInt64 k{ version.load(std::memory_order_acquire) };

  if (k == 0) return std::nullopt;
  
  const std::size_t idx{ static_cast<std::size_t>(k & 1u) };

  return m_frameSnapshotBuffer[idx];
}
