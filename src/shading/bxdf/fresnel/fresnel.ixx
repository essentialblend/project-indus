export module indus.shading.fresnel;

import std;

import indus.core.types;

export class Fresnel
{
public:
  virtual ~Fresnel() = default;
  [[nodiscard]] virtual Float evaluate(Float) const noexcept = 0;
};

export class FresnelNoOp final : public Fresnel 
{
public:
  [[nodiscard]] Float evaluate([[maybe_unused]] Float incidentCosineTheta) const noexcept override
  {
    return 1.0;
  }
};