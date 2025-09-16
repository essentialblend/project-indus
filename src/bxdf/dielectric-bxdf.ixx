export module dielectricbxdf;

import std;
import bxdf;
import vector;
import fresneldielectric;
import types;

export class DielectricBxDF final : public BxDF 
{
public:
  DielectricBxDF(const ColorRGB&, const ColorRGB&, Float, Float) noexcept;


  [[nodiscard]] ColorRGB evaluate(const Vec3f&, const Vec3f& unitLocalW_i) const noexcept override;


  [[nodiscard]] std::tuple<Vec3f, Float, ColorRGB, BxDFType> sample(const Vec3f&, const Point2f& s) const override;


  [[nodiscard]] Float PDF(const Vec3f&, const Vec3f&) const noexcept override;


  [[nodiscard]] BxDFType type() const noexcept override;


private:
  ColorRGB m_reflectance{};
  ColorRGB m_transmittance{};
  Float m_etaI{};
  Float m_etaT{};
};

DielectricBxDF::DielectricBxDF(const ColorRGB& reflectance, const ColorRGB& transmittance, Float etaI, Float etaT) noexcept : BxDF(BxDFType::Specular | BxDFType::Reflection | BxDFType::Transmission), m_reflectance{ reflectance }, m_transmittance{ transmittance }, m_etaI{ etaI }, m_etaT{ etaT } {}

ColorRGB DielectricBxDF::evaluate(const Vec3f&, const Vec3f&) const noexcept
{
  return ColorRGB{};
}

std::tuple<Vec3f, Float, ColorRGB, BxDFType> DielectricBxDF::sample(const Vec3f& unitLocalW_o, const Point2f& uniformSample) const
{
  const Float cosO{ unitLocalW_o[2] };

  // oriented m_etaI/m_etaT
  const Float Fr{ FresnelDielectric{ m_etaI, m_etaT }.evaluate(cosO) };

  // Pick lobe with pR=Fr, pT=1-Fr
  if (uniformSample[0] < Fr)
  { 
    // REFLECT
    if (cosO <= 0.0) return { Vec3f{0}, Float(0.0), ColorRGB{0}, BxDFType::Reflection | BxDFType::Specular };
    const Vec3f wi{ -unitLocalW_o[0], -unitLocalW_o[1], cosO };

    // no Fr in f
    return { wi, Float(1.0), m_reflectance, BxDFType::Reflection | BxDFType::Specular };
  }

  // TRANSMIT
  const Float eta{ m_etaI / m_etaT };
  const Float sin2O{ Float(std::max(0.0, 1.0 - cosO * cosO)) };
  const Float sin2T{ Float(eta * eta * sin2O) };
  
  if (sin2T >= 1.0) return { Vec3f{}, Float(0.0), ColorRGB{}, BxDFType::Transmission | BxDFType::Specular };

  // below surface
  Float cosT{ Float(-std::sqrt(std::max(0.0, 1.0 - sin2T))) };
  const Vec3f wi{ -eta * unitLocalW_o[0], -eta * unitLocalW_o[1], cosT };

  // PBRT radiance-transport scale (no 1/|cosT|)
  const Float scale{ (eta * eta) * std::abs(wi[2]) / std::abs(cosO) };

  return { wi, Float(1.0), m_transmittance * scale, BxDFType::Transmission | BxDFType::Specular };
}

Float DielectricBxDF::PDF(const Vec3f&, const Vec3f&) const noexcept
{
  return 0.0;
}

BxDFType DielectricBxDF::type() const noexcept
{
  return BxDFType::Specular | BxDFType::Reflection | BxDFType::Transmission;
}