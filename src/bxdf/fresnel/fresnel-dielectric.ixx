export module fresneldielectric;

import std;
import fresnel;
import vector;
import types;

export class FresnelDielectric final : public Fresnel
{
public:
  FresnelDielectric(Float, Float) noexcept;

  [[nodiscard]] Float evaluate(Float) const noexcept override;

private:
  Float m_incidentEta{};
  Float m_transmittedEta{};
};

FresnelDielectric::FresnelDielectric(Float incidentEta, Float transmittedEta) noexcept : m_incidentEta{ incidentEta }, m_transmittedEta{ transmittedEta } {}

Float FresnelDielectric::evaluate(Float incidentCosineTheta) const noexcept
{
  incidentCosineTheta = std::clamp(incidentCosineTheta, Float(- 1.0), Float(1.0));

  bool isLightEntering{ incidentCosineTheta > 0.0 };

  Float etaI{ isLightEntering ? m_incidentEta : m_transmittedEta };
  Float etaT{ isLightEntering ? m_transmittedEta : m_incidentEta };

  Float incidentSinTheta{ Float(std::sqrt(std::max(0.0, 1.0 - (incidentCosineTheta * incidentCosineTheta)))) };
  Float transmittedSinTheta{ (etaI / etaT) * incidentSinTheta };

  if (transmittedSinTheta >= 1.0) return 1.0;

  Float transmittedCosineTheta{ Float(std::sqrt(std::max(0.0, (1.0 - (transmittedSinTheta * transmittedSinTheta))))) };

  Float Rs{ ((etaI * incidentCosineTheta) - (etaT * transmittedCosineTheta)) / ((etaI * incidentCosineTheta) + (etaT * transmittedCosineTheta)) };

  Float Rp{ ((etaT * incidentCosineTheta) - (etaI * transmittedCosineTheta)) / ((etaT * incidentCosineTheta) + (etaI * transmittedCosineTheta)) };

  return Float(0.5 * ((Rs * Rs) + (Rp * Rp)));
}