export module fresneldielectric;

import std;
import fresnel;
import vec3;

export class FresnelDielectric final : public Fresnel
{
public:
  FresnelDielectric(double incidentEta, double transmittedEta) noexcept : m_incidentEta{ incidentEta }, m_transmittedEta{ transmittedEta } {}

  [[nodiscard]] auto evaluate(double incidentCosineTheta) const noexcept -> double override
  {
    incidentCosineTheta = std::clamp(incidentCosineTheta, -1.0, 1.0);

    bool isLightEntering{ incidentCosineTheta > 0.0 };

    double etaI{ isLightEntering ? m_incidentEta : m_transmittedEta };
    double etaT{ isLightEntering ? m_transmittedEta : m_incidentEta };

    double incidentSinTheta{ std::sqrt(std::max(0.0, 1.0 - (incidentCosineTheta * incidentCosineTheta))) };
    double transmittedSinTheta{ (etaI / etaT) * incidentSinTheta };

    if (transmittedSinTheta >= 1.0) return 1.0;

    double transmittedCosineTheta{ std::sqrt(std::max(0.0, (1.0 - (transmittedSinTheta * transmittedSinTheta)))) };

    double Rs{ ((etaI * incidentCosineTheta) - (etaT * transmittedCosineTheta)) / ((etaI * incidentCosineTheta) + (etaT * transmittedCosineTheta)) };

    double Rp{ ((etaT * incidentCosineTheta) - (etaI * transmittedCosineTheta)) / ((etaT * incidentCosineTheta) + (etaI * transmittedCosineTheta)) };

    return (0.5 * ((Rs * Rs) + (Rp * Rp)));
  }

private:
  double m_incidentEta{};
  double m_transmittedEta{};
};