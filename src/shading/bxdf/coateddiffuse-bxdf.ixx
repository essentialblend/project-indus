export module indus.shading.coateddiffuse_bxdf;

import indus.core.types;
import indus.core.colorrgb;
import indus.core.math.fp.ii;
import indus.core.math.trig.iii;
import indus.core.math.algebra.iv;
import indus.core.math.float_constants;
import indus.core.geom.util;

import indus.shading.bxdf;
import indus.shading.fresnel_dielectric;

import indus.sampling.constructs;
import indus.sampling.util;

// Deeper understanding of the GGX/NDFs deferred until LayeredBxDF is implemented
export class CoatedDiffuseBxDF final : public BxDF
{
public:
  CoatedDiffuseBxDF(const ColorRGB& baseReflectance, Float coatEta, Float coatRoughness) noexcept;

  [[nodiscard]] ColorRGB evaluate(const Vec3f& unitW_oLocal, const Vec3f& unitW_iLocal) const noexcept override;

  [[nodiscard]] std::optional<BSDFSample> sample(const Vec3f& unitW_oLocal, const Point2f& uniformSample) const noexcept override;

  [[nodiscard]] Float PDF(const Vec3f& unitW_oLocal, const Vec3f& unitW_iLocal) const noexcept override;
  [[nodiscard]] BxDFType type() const noexcept override;

private:
  ColorRGB m_baseReflectance{};
  Float m_coatEta{ 1.5 };
  Float m_coatRoughness{};
};

CoatedDiffuseBxDF::CoatedDiffuseBxDF(const ColorRGB& baseReflectance, Float coatEta, Float coatRoughness) noexcept : BxDF{ BxDFType::Reflection | BxDFType::Diffuse | BxDFType::Glossy },
  m_baseReflectance{ baseReflectance }, m_coatEta{ coatEta }, m_coatRoughness{ coatRoughness } {}

ColorRGB CoatedDiffuseBxDF::evaluate(const Vec3f& unitW_oLocal, const Vec3f& unitW_iLocal) const noexcept
{
  if (!isFinite(unitW_oLocal) || !isFinite(unitW_iLocal) || !isFinite(m_baseReflectance)) return ColorRGB{};
  if (!sameHemisphereLocal(unitW_oLocal, unitW_iLocal)) return ColorRGB{};

  const Float cosInc{ absCosineThetaLocal(unitW_iLocal) };
  const Float cosOut{ absCosineThetaLocal(unitW_oLocal) };

  if (cosOut <= Float{} || cosInc <= Float{}) return ColorRGB{};
  
  const Vec3f W_h{ normalize(unitW_iLocal + unitW_oLocal) };
  
  if (!isFinite(W_h) || W_h[2] <= Float{}) return ColorRGB{};

  const FresnelDielectric fresnel{ Float{ 1 }, m_coatEta };

  const Float F{ fresnel.evaluate(computeDot(unitW_iLocal, W_h)) };
  const Float D{ trowbridgeReitz_D(W_h, microfacetAlphaFromRoughness(m_coatRoughness)) };
  const Float G{ smithGGX_G(unitW_oLocal, unitW_iLocal, microfacetAlphaFromRoughness(m_coatRoughness)) };
  
  const Float denom{ 4 * cosInc * cosOut };
  const ColorRGB fCoat{ (denom > Float{}) ? ColorRGB{ (F * D * G) / denom } : ColorRGB{} };

  const Float R_o{ fresnel.evaluate(cosOut) };
  const Float R_i{ fresnel.evaluate(cosInc) };
  const ColorRGB fBase{ m_baseReflectance * kInvPi * ((Float{ 1 } - R_o) * (Float{ 1 } - R_i)) };

  return fCoat + fBase;
}

std::optional<BSDFSample> CoatedDiffuseBxDF::sample(const Vec3f& unitW_oLocal, const Point2f& uniformSample) const noexcept
{
  if (!isFinite(unitW_oLocal) || !isFinite(m_baseReflectance)) return std::nullopt;

  const Float cosOut{ absCosineThetaLocal(unitW_oLocal) };
  if (cosOut <= Float{}) return std::nullopt;

  const FresnelDielectric fresnel{ Float{ 1 }, m_coatEta };
  const Float wCoat{ fresnel.evaluate(cosOut) };
  const Float wBase{ Float{ 1 } - wCoat };

  Vec3f unitW_iLocal{};

  if (uniformSample[0] < wCoat)
  {
    if (!(wCoat > Float{})) return std::nullopt;

    // Remap the selected coat interval before sampling the lobe.
    const Float remappedX{ std::min(uniformSample[0] / wCoat, oneMinusEpsFloat) };
    const Point2f lobeSample{ remappedX, uniformSample[1] };
    Vec3f W_h{ sampleGGXHalfVector(lobeSample, microfacetAlphaFromRoughness(m_coatRoughness)) };

    if (!isFinite(W_h) || W_h[2] <= Float{}) return std::nullopt;

    const Float W_oDotW_h{ computeDot(unitW_oLocal, W_h) };
    
    if (W_oDotW_h <= Float{}) return std::nullopt;

    unitW_iLocal = normalize((Float{ 2 } * W_oDotW_h * W_h) - unitW_oLocal);

    if (!isFinite(unitW_iLocal) || !sameHemisphereLocal(unitW_oLocal, unitW_iLocal)) return std::nullopt;
  }
  else 
  {
    if (!(wBase > Float{})) return std::nullopt;

    // Remap the selected base interval before sampling the lobe.
    const Float remappedX{ std::min((uniformSample[0] - wCoat) / wBase, oneMinusEpsFloat) };
    const Point2f lobeSample{ remappedX, uniformSample[1] };
    Vec3f W_i{ normalize(genCosineWeightedHemisphereVec(lobeSample)) };
    
    if (!isFinite(W_i)) return std::nullopt;
    if (cosineThetaLocal(unitW_oLocal) < Float{}) W_i[2] *= Float{ -1 };
    if (!sameHemisphereLocal(unitW_oLocal, W_i)) return std::nullopt;
    
    unitW_iLocal = W_i;
  }

  const Float PDF{ this->PDF(unitW_oLocal, unitW_iLocal) };
  
  if (!isFinite(PDF) || PDF <= Float{}) return std::nullopt;

  const ColorRGB BRDF{ evaluate(unitW_oLocal, unitW_iLocal) };
  if (!isFinite(BRDF)) return std::nullopt;

  return BSDFSample{ unitW_iLocal, std::nullopt, BRDF, PDF, BxDFType::Reflection | BxDFType::Diffuse | BxDFType::Glossy };
}

Float CoatedDiffuseBxDF::PDF(const Vec3f& unitW_oLocal, const Vec3f& unitW_iLocal) const noexcept
{
  if (!isFinite(unitW_oLocal) || !isFinite(unitW_iLocal)) return Float{};
  if (!sameHemisphereLocal(unitW_oLocal, unitW_iLocal)) return Float{};

  const Float cosOut{ absCosineThetaLocal(unitW_oLocal) };
  const Float cosInc{ absCosineThetaLocal(unitW_iLocal) };

  if (cosOut <= Float{} || cosInc <= Float{}) return Float{};

  const FresnelDielectric fresnel{ Float{ 1 }, m_coatEta };
  const Float wCoat{ fresnel.evaluate(cosOut) };
  const Float wBase{ Float{ 1 } - wCoat };

  const Float PDFBase{ cosInc * kInvPi };

  Vec3f W_h{ normalize(unitW_iLocal + unitW_oLocal) };
  
  if (!isFinite(W_h) || W_h[2] <= Float{}) return PDFBase * wBase;

  const Float alpha{ microfacetAlphaFromRoughness(m_coatRoughness) };
  const Float D{ trowbridgeReitz_D(W_h, alpha) };
  const Float cosH{ absCosineThetaLocal(W_h) };
  const Float W_oDotW_h{ std::max(computeDot(unitW_oLocal, W_h), Float{}) };

  Float PDFCoat{};

  if (cosH > Float{} && W_oDotW_h > Float{}) 
  {
    const Float PDFW_h{ D * cosH };
    PDFCoat = PDFW_h / (4 * W_oDotW_h);
  }

  return (wCoat * PDFCoat) + (wBase * PDFBase);
}

BxDFType CoatedDiffuseBxDF::type() const noexcept
{
  return BxDFType::Reflection | BxDFType::Diffuse | BxDFType::Glossy;
}
