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
  std::mutex m_mutex{};
  std::optional<FrameSnapshot> m_pendingImage{};
  std::optional<FrameSnapshot> m_pendingProgress{};
};

void FrameMailbox::publishFrameSnapshot(FrameSnapshot frameSnapshot) noexcept
{
  std::scoped_lock lock{ m_mutex };

  if (!frameSnapshot.image.getP8().empty())
    m_pendingImage = std::move(frameSnapshot);
  else
    m_pendingProgress = std::move(frameSnapshot);
}

std::optional<FrameSnapshot> FrameMailbox::tryConsume() noexcept
{
  std::scoped_lock lock{ m_mutex };

  if (m_pendingImage)
  {
    std::optional<FrameSnapshot> snapshot{ std::move(m_pendingImage) };
    m_pendingImage.reset();
    return snapshot;
  }

  if (m_pendingProgress)
  {
    std::optional<FrameSnapshot> snapshot{ std::move(m_pendingProgress) };
    m_pendingProgress.reset();
    return snapshot;
  }

  return std::nullopt;
}
