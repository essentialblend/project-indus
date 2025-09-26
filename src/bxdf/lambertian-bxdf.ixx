export module lambertianBxDF;

import std;
import vector;
import bxdf;
import constructs;
import types;
import colorrgb;
import core_diag;
import core_sampling_util;
import bsdf_new;

export class LambertianBxDF final : public BxDF
{
public:
  explicit LambertianBxDF(const ColorRGB& albedo) noexcept;

  [[nodiscard]] virtual ColorRGB evaluate([[maybe_unused]] const Vec3f&, [[maybe_unused]] const Vec3f&) const noexcept override;
  
  [[nodiscard]] virtual std::optional<BSDFSample> sample([[maybe_unused]] const Vec3f&, const Point2f&) const override;
  
  [[nodiscard]] virtual Float PDF([[maybe_unused]] const Vec3f&, [[maybe_unused]] const Vec3f&) const noexcept override;
  
  [[nodiscard]] virtual BxDFType type() const noexcept override;

private:
  ColorRGB m_albedo{};
};

LambertianBxDF::LambertianBxDF(const ColorRGB& albedo) noexcept : BxDF{ BxDFType::Reflection | BxDFType::Diffuse }, m_albedo{ albedo } {}

// Evaluate contribution for outgoing w_o
[[nodiscard]] ColorRGB LambertianBxDF::evaluate([[maybe_unused]] const Vec3f& unitW_oLocal, [[maybe_unused]] const Vec3f& unitW_iLocal) const noexcept
{
  if (!isFinite(unitW_oLocal) || !isFinite(unitW_iLocal) || !isFinite(m_albedo)) return ColorRGB{};
  if (unitW_oLocal[2] <= Float(0.0) || unitW_iLocal[2] <= Float(0.0)) return ColorRGB{};

  constexpr Float inversePi{ Float(1) / std::numbers::pi_v<Float> };
  return m_albedo * inversePi;
}

// Sample an incoming w_i
[[nodiscard]] std::optional<BSDFSample> LambertianBxDF::sample([[maybe_unused]] const Vec3f& unitW_oLocal, const Point2f& uniformSample) const
{
  if (!isFinite(unitW_oLocal) || !isFinite(m_albedo)) return std::nullopt;
  if (unitW_oLocal[2] <= Float(0)) return std::nullopt;

  Vec3f unitW_iLocal{ normalize(genCosineWeightedHemisphereVec(uniformSample)) };
  if (!isFinite(unitW_iLocal) || unitW_iLocal[2] <= Float(0)) return std::nullopt;

  constexpr Float inversePi{ Float(1) / std::numbers::pi_v<Float> };
  const Float PDF{ unitW_iLocal[2] * inversePi };
  const ColorRGB BRDF{ m_albedo * inversePi };

  if (!isFinite(PDF) || PDF <= Float(0) || !isFinite(BRDF)) return std::nullopt;

  return BSDFSample{ unitW_iLocal, std::nullopt, BRDF, PDF, BxDFType::Reflection | BxDFType::Diffuse };
}

[[nodiscard]] Float LambertianBxDF::PDF([[maybe_unused]] const Vec3f& unitW_oLocal, [[maybe_unused]] const Vec3f& unitW_iLocal) const noexcept
{
  if (!isFinite(unitW_oLocal) || !isFinite(unitW_iLocal)) return Float(0);
  if (unitW_oLocal[2] <= Float(0) || unitW_iLocal[2] <= Float(0)) return Float(0);

  constexpr Float inversePi{ Float(1) / std::numbers::pi_v<Float> };
  const Float cosI{ std::max(Float(0), unitW_iLocal[2]) };

  return cosI * inversePi;
}

[[nodiscard]] BxDFType LambertianBxDF::type() const noexcept
{
  return BxDFType::Reflection | BxDFType::Diffuse;
}