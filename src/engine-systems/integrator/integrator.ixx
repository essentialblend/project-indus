export module integrator;

import std;
import scene;
import engineconstructs;

export class Integrator 
{
public:
  virtual ~Integrator() = default;
  virtual void render(const Scene&) = 0;

protected:
  explicit Integrator(const RuntimeComponents& renderRuntimeComponents) noexcept;

protected:
  Integrator() noexcept = default;
  Integrator(const Integrator&) = delete;
  Integrator& operator=(const Integrator&) = delete;
  Integrator(Integrator&&) = delete;
  Integrator& operator=(Integrator&&) = delete;

  const RuntimeComponents& m_runtimeComponents{};
};

Integrator::Integrator(const RuntimeComponents& renderRuntimeComponents) noexcept : m_runtimeComponents{ renderRuntimeComponents } {}