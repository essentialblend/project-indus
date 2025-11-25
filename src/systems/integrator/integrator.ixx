export module indus.integrator.base;

import std;

import indus.stats.constructs;

import indus.integrator.constructs;

import indus.scene;

export class Integrator 
{
public:
  virtual ~Integrator() = default;
  virtual void render(const Scene&, std::stop_token) = 0;

  void setDisplayConsumer(DisplayConsumer displayConsumerFnctr) noexcept;

  [[nodiscard]] virtual std::string toString() const = 0;
  [[nodiscard]] virtual std::string getSchedulerString() const = 0;

  [[nodiscard]] const RenderStats& getRenderStats() const noexcept;

protected: 
  RenderStats m_renderStats{};

  Integrator() noexcept = default;
  Integrator(const Integrator&) = delete;
  Integrator& operator=(const Integrator&) = delete;
  Integrator(Integrator&&) = delete;
  Integrator& operator=(Integrator&&) = delete;

  DisplayConsumer m_displayConsumer{ nullptr };
};

void Integrator::setDisplayConsumer(DisplayConsumer displayConsumerFnctr) noexcept
{
  m_displayConsumer = std::move(displayConsumerFnctr);
}

const RenderStats& Integrator::getRenderStats() const noexcept
{
  return m_renderStats;
}
