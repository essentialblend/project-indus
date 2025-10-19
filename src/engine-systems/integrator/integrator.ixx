export module integrator;

import std;
import scene;

export class Integrator 
{
public:
  virtual ~Integrator() = default;
  virtual void render(const Scene&) = 0;

protected:
  Integrator() noexcept = default;
  Integrator(const Integrator&) = delete;
  Integrator& operator=(const Integrator&) = delete;
  Integrator(Integrator&&) = delete;
  Integrator& operator=(Integrator&&) = delete;
};
