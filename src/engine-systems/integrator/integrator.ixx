export module integrator;

import std;
import scene;
import engineconstructs;

export class Integrator 
{
public:
  virtual ~Integrator() = default;
  virtual void render(const Scene&) = 0;

  void setDisplayConsumer(DisplayConsumer displayConsumerFnctr) noexcept;

protected: 
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
