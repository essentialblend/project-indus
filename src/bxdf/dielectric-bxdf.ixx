export module dielectricbxdf;

import std;
import bxdf;
import vector;
import fresneldielectric;
import types;
import constructs;
import core_diag;
import samplingutil;
import mathtrig;
import mathfp;
import mathgeometry;

export class DielectricBxDF final : public BxDF
{
public:
  DielectricBxDF(const ColorRGB&, const ColorRGB&, Float, Float) noexcept;
  [[nodiscard]] ColorRGB evaluate(const Vec3f&, const Vec3f&) const noexcept override;
  [[nodiscard]] std::optional<BSDFSample> sample(const Vec3f& unitW_oLocal, const Point2f& uniformSample) const noexcept override;
  [[nodiscard]] Float PDF(const Vec3f&, const Vec3f&) const noexcept override;
  [[nodiscard]] BxDFType type() const noexcept override;

private:
  ColorRGB m_reflectance{};
  ColorRGB m_transmittance{};
  Float m_etaI{};
  Float m_etaT{};
};

DielectricBxDF::DielectricBxDF(const ColorRGB& reflectance, const ColorRGB& transmittance, Float etaI, Float etaT) noexcept : BxDF(BxDFType::Specular | BxDFType::Reflection | BxDFType::Transmission),
  m_reflectance{ reflectance }, m_transmittance{ transmittance }, m_etaI{ etaI }, m_etaT{ etaT } {}

ColorRGB DielectricBxDF::evaluate([[maybe_unused]] const Vec3f& unitW_oLocal, [[maybe_unused]] const Vec3f& unitW_iLocal) const noexcept
{
  return ColorRGB{};
}

Float DielectricBxDF::PDF([[maybe_unused]] const Vec3f& unitW_oLocal, [[maybe_unused]] const Vec3f& unitW_iLocal) const noexcept
{
  return Float{};
}

BxDFType DielectricBxDF::type() const noexcept 
{
  return BxDFType::Specular | BxDFType::Reflection | BxDFType::Transmission;
}

std::optional<BSDFSample> DielectricBxDF::sample(const Vec3f& unitW_oLocal, const Point2f& uniformSample) const noexcept
{
  if (!isFinite(unitW_oLocal) || !isFinite(m_reflectance) || !isFinite(m_transmittance)) return std::nullopt;

  constexpr Float eps{ 1e-8f };
  const Float cosWo{ cosineThetaLocal(unitW_oLocal) };

  const Float R{ FresnelDielectric{ m_etaI, m_etaT }.evaluate(cosWo) };
  const Float T{ Float(1) - R };
  
  Float pr{ R }; Float pt{ T };
  if (pr + pt <= 0) return std::nullopt;

  BSDFSample s{};
  const Float uc{ uniformSample[0] };

  // reflection branch
  if (uc < pr / (pr + pt)) 
  {
    const Vec3f wiL{ -unitW_oLocal[0], -unitW_oLocal[1], unitW_oLocal[2] };
    if (!sameHemisphere(unitW_oLocal, wiL)) return std::nullopt;

    const Float cosR{ absCosineThetaLocal(wiL) };
    if (!isFinite(wiL) || cosR <= 0) return std::nullopt;
    
    s.unitW_iLocal = wiL;
    s.BRDF = m_reflectance * (R / std::max(cosR, eps));
    s.PDF = pr / (pr + pt);
    s.flags = BxDFType::Specular | BxDFType::Reflection;
  }
  else
  {
    // transmission branch
    const Float etap{ (unitW_oLocal[2] >= 0) ? (m_etaI / m_etaT) : (m_etaT / m_etaI) };
    auto tr{ refractLocal(unitW_oLocal, etap) };

    if (!tr) return std::nullopt;

    const Vec3f wiL{ tr->unitW_iLocal };
    
    if (sameHemisphere(unitW_oLocal, wiL)) return std::nullopt;
    
    const Float cosT{ absCosineThetaLocal(wiL) };
    
    if (!isFinite(wiL) || cosT <= 0) return std::nullopt;

    s.unitW_iLocal = wiL;
    s.BRDF = m_transmittance * ((T / std::max(cosT, eps)) / sqr(etap));
    s.PDF = pt / (pr + pt);
    s.flags = BxDFType::Specular | BxDFType::Transmission;
  }
  
  return s;
}
