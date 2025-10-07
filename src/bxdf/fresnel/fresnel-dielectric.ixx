export module fresneldielectric;

import std;
import fresnel;
import vector;
import types;

import mathfp;

export class FresnelDielectric final : public Fresnel
{
public:
  FresnelDielectric(Float, Float) noexcept;

  [[nodiscard]] Float evaluate(Float) const noexcept override;

private:
  Float m_etaIncident{};
  Float m_etaTransmittance{};
};

FresnelDielectric::FresnelDielectric(Float incidentEta, Float transmittedEta) noexcept : m_etaIncident{ incidentEta }, m_etaTransmittance{ transmittedEta } {}

Float FresnelDielectric::evaluate(Float incidentCosineTheta) const noexcept
{
  incidentCosineTheta = clampUnit(incidentCosineTheta);

  const bool isLightEntering{ incidentCosineTheta > Float{ 0.0 } };

  const Float etaI{ isLightEntering ? m_etaIncident : m_etaTransmittance };
  const Float etaT{ isLightEntering ? m_etaTransmittance : m_etaIncident };

  if (!isLightEntering) incidentCosineTheta = -incidentCosineTheta;

  Float incidentSinTheta{ safeSqrt(Float{ 1.0 } - sqr(incidentCosineTheta)) };
  Float transmittedSinTheta{ (etaI / etaT) * incidentSinTheta };

  if (transmittedSinTheta >= Float{ 1.0 }) return Float{ 1.0 };

  const Float transmittedCosineTheta{ safeSqrt(Float{ 1.0 } - sqr(transmittedSinTheta)) };

  const Float r_sDenom{ sumOfProducts(etaI, incidentCosineTheta, etaT, transmittedCosineTheta) };
  const Float r_pDenom{ sumOfProducts(etaT, incidentCosineTheta, etaI, transmittedCosineTheta) };

  if (r_sDenom == Float{ 0.0 } || r_pDenom == Float{ 0.0 }) return Float{ 1.0 };

  const Float r_s{ differenceOfProducts(etaI, incidentCosineTheta, etaT, transmittedCosineTheta) / r_sDenom };

  const Float r_p{ differenceOfProducts(etaT, incidentCosineTheta, etaI, transmittedCosineTheta) / r_pDenom };

  return Float{ Float{ 0.5 } * (sqr(r_s) + sqr(r_p)) };
}