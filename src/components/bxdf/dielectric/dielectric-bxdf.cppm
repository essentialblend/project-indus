import dielectricbxdf;

import std;
import vec3;
import fresneldielectric;
import bxdf;

DielectricBxDF::DielectricBxDF(const ColorRGB& reflectance,
  const ColorRGB& transmittance, double etaI, double etaT) noexcept : BxDF(BxDFType::Specular | BxDFType::Reflection | BxDFType::Transmission), m_reflectance{ reflectance }, m_transmittance{ transmittance }, m_etaI{ etaI }, m_etaT{ etaT } {}

auto DielectricBxDF::evaluate(const Vec3&, const Vec3&) const noexcept -> ColorRGB
{
  return ColorRGB{ 0 };
}

auto DielectricBxDF::sample(const Vec3& unitLocalW_o, const Sample2D& uniformSample) const -> std::tuple<Vec3, double, ColorRGB, BxDFType>
{
  const double cosO = unitLocalW_o[2];

  // oriented m_etaI/m_etaT
  const double Fr = FresnelDielectric{ m_etaI, m_etaT }.evaluate(cosO); 

  // Pick lobe with pR=Fr, pT=1-Fr
  if (uniformSample.u < Fr) { // REFLECT
    if (cosO <= 0.0) return { Vec3{0},0.0,ColorRGB{0}, BxDFType::Reflection | BxDFType::Specular };
    const Vec3 wi{ -unitLocalW_o[0], -unitLocalW_o[1], cosO };
    
    // no Fr in f
    return { wi, 1.0, m_reflectance, BxDFType::Reflection | BxDFType::Specular }; 
  }

  // TRANSMIT
  const double eta = m_etaI / m_etaT;                         
  const double sin2O = std::max(0.0, 1.0 - cosO * cosO);
  const double sin2T = eta * eta * sin2O;
  if (sin2T >= 1.0)   return { Vec3{0},0.0,ColorRGB{0}, BxDFType::Transmission | BxDFType::Specular };

  // below surface
  double cosT = -std::sqrt(std::max(0.0, 1.0 - sin2T));          
  const Vec3 wi{ -eta * unitLocalW_o[0], -eta * unitLocalW_o[1], cosT };

  // PBRT radiance-transport scale (no 1/|cosT|)
  const double scale = (eta * eta) * std::abs(wi[2]) / std::abs(cosO);
  
  return { wi, 1.0, m_transmittance * scale, BxDFType::Transmission | BxDFType::Specular };
}



auto DielectricBxDF::PDF(const Vec3&, const Vec3&) const noexcept -> double 
{
  return 0.0;
}


auto DielectricBxDF::type() const noexcept -> BxDFType 
{
  return BxDFType::Specular | BxDFType::Reflection | BxDFType::Transmission;
}

