export module fresnel;

import std;
import vec3;

export class Fresnel
{
public:
  virtual ~Fresnel() = default;
  [[nodiscard]] virtual auto evaluate(double incidentCosineTheta) const noexcept -> double = 0;
};

export class FresnelNoOp final : public Fresnel 
{
public:
  [[nodiscard]] auto evaluate([[maybe_unused]] double incidentCosineTheta) const noexcept -> double override
  {
    return 1.0;
  }
};