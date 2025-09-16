export module lambertianBxDF;

import std;
import vector;
import bxdf;
import constructs;
import types;
import colorrgb;
import core_sampling_util;

export class LambertianBxDF final : public BxDF
{
public:
  explicit LambertianBxDF(const ColorRGB& albedo) noexcept;

  [[nodiscard]] virtual ColorRGB evaluate([[maybe_unused]] const Vec3f&, [[maybe_unused]] const Vec3f&) const noexcept override;
  
  [[nodiscard]] virtual std::tuple<Vec3f, Float, ColorRGB, BxDFType> sample([[maybe_unused]] const Vec3f&, const Point2f&) const override;
  
  [[nodiscard]] virtual Float PDF([[maybe_unused]] const Vec3f&, [[maybe_unused]] const Vec3f&) const noexcept override;
  
  [[nodiscard]] virtual BxDFType type() const noexcept override;

private:
  ColorRGB m_albedo{};
};

LambertianBxDF::LambertianBxDF(const ColorRGB& albedo) noexcept : BxDF{ BxDFType::Reflection | BxDFType::Diffuse }, m_albedo{ albedo } {}

// Evaluate contribution for outgoing w_o
[[nodiscard]] ColorRGB LambertianBxDF::evaluate([[maybe_unused]] const Vec3f& unitLocalW_o, [[maybe_unused]] const Vec3f& unitLocalW_i) const noexcept
{
  if (unitLocalW_o[2] <= 0.0 || unitLocalW_i[2] <= 0.0) return ColorRGB{ 0 };

  return (m_albedo * (1 / std::numbers::pi_v<Float>));
}

// Sample an incoming w_i
[[nodiscard]] std::tuple<Vec3f, Float, ColorRGB, BxDFType> LambertianBxDF::sample([[maybe_unused]] const Vec3f& unitLocalW_o, const Point2f& uniformSample) const
{
  // Refuse degenerate camera directions
  if (unitLocalW_o[2] <= 0.0) return { Vec3f{0, 0, 0}, Float(0.0), ColorRGB{0.0}, type() };

  Vec3f unitLocalW_i{ normalize(genCosineWeightedHemisphereVec(uniformSample)) };

  Float PDFValue{ PDF(unitLocalW_o, unitLocalW_i) };
  ColorRGB BRDFValue{ evaluate(unitLocalW_o, unitLocalW_i) };

  return { unitLocalW_i, PDFValue, BRDFValue, type() };
}

[[nodiscard]] Float LambertianBxDF::PDF([[maybe_unused]] const Vec3f& unitLocalW_o, [[maybe_unused]] const Vec3f& unitLocalW_i) const noexcept
{
  if (unitLocalW_o[2] <= 0.0 || unitLocalW_i[2] <= 0.0) return 0.0;

  const auto cosineTheta{ computeDot(unitLocalW_i, Vec3f{0, 0, 1}) };
  return static_cast<Float>(cosineTheta / std::numbers::pi_v<Float>);
}

[[nodiscard]] BxDFType LambertianBxDF::type() const noexcept
{
  return BxDFType::Reflection | BxDFType::Diffuse;
}