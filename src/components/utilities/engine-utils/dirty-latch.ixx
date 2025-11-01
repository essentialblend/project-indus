export module dirtylatch;

import std;

export class DirtyLatch final
{
public:
  void publish() noexcept;
  bool consume() noexcept;
  bool peek() const noexcept;
private:
  std::atomic<bool> m_flag{ false };
};

void DirtyLatch::publish() noexcept
{
  m_flag.store(true, std::memory_order_release);
}

bool DirtyLatch::consume() noexcept
{
  return m_flag.exchange(false, std::memory_order_acq_rel);
}

bool DirtyLatch::peek() const noexcept
{
  return m_flag.load(std::memory_order_acquire);
}

