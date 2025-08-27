import lambertianBxDF;

import std;
import core_util;
import core_sampling_util;
import vec3;
import bxdf;

LambertianBxDF::LambertianBxDF(const ColorRGB& albedo) noexcept : BxDF{ BxDFType::Reflection | BxDFType::Diffuse }, m_albedo{ albedo } {}

// Evaluate contribution for outgoing w_o
[[nodiscard]] auto LambertianBxDF::evaluate([[maybe_unused]] const Vec3& unitLocalW_o, [[maybe_unused]] const Vec3& unitLocalW_i) const noexcept -> ColorRGB
{
  if (unitLocalW_o[2] <= 0.0 || unitLocalW_o[2] <= 0.0) return ColorRGB{ 0 };

  return (m_albedo * (1 / UPi));
}

// Sample an incoming w_i
[[nodiscard]] auto LambertianBxDF::sample([[maybe_unused]] const Vec3& unitLocalW_o, const Sample2D& uniformSample) const -> std::tuple<Vec3, double, ColorRGB, BxDFType>
{
  // Refuse degenerate camera directions
  if (unitLocalW_o[2] <= 0.0) return { Vec3{0, 0, 0}, 0.0, ColorRGB{0}, type() };

  Vec3 unitLocalW_i{ getUnit(genCosineWeightedHemisphereVec(uniformSample.u, uniformSample.v)) };

  double PDFValue{ PDF(unitLocalW_o, unitLocalW_i) };
  ColorRGB BRDFValue{ evaluate(unitLocalW_o, unitLocalW_i) };
  
  return { unitLocalW_i, PDFValue, BRDFValue, type() };
}

[[nodiscard]] auto LambertianBxDF::PDF([[maybe_unused]] const Vec3& unitLocalW_o, [[maybe_unused]] const Vec3& unitLocalW_i) const noexcept -> double
{
  if (unitLocalW_o[2] <= 0.0 || unitLocalW_i[2] <= 0.0) return 0.0;

  const auto cosineTheta{ computeDot(unitLocalW_i, Vec3{0, 0, 1}) };
  return static_cast<double>(cosineTheta / UPi);
}

[[nodiscard]] auto LambertianBxDF::type() const noexcept -> BxDFType 
{
  return BxDFType::Reflection | BxDFType::Diffuse;
}