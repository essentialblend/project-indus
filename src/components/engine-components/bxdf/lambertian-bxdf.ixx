export module lambertianBxDF;

import std;
import vector;
import bxdf;
import samplingconstructs;
import types;
import colorrgb;
import mathutil;
import mathconstants;
import samplingutil;
import mathfp;
import mathalgebra;
import mathgeometry;
import mathtrig;
import mathutil;
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

  if (!sameHemisphereLocal(unitW_oLocal, unitW_iLocal)) return ColorRGB{};

  return m_albedo * kInvPi;
}

// Sample an incoming w_i
[[nodiscard]] std::optional<BSDFSample> LambertianBxDF::sample([[maybe_unused]] const Vec3f& unitW_oLocal, const Point2f& uniformSample) const
{
  if (!isFinite(unitW_oLocal) || !isFinite(m_albedo)) return std::nullopt;
  if (absCosineThetaLocal(unitW_oLocal) <= Float{}) return std::nullopt;

  Vec3f unitW_iLocal{ normalize(genCosineWeightedHemisphereVec(uniformSample)) };

  if (!isFinite(unitW_iLocal)) return std::nullopt;
  if (cosineThetaLocal(unitW_oLocal) < Float{}) unitW_iLocal[2] *= Float{ -1 };

  const Float PDF{ absCosineThetaLocal(unitW_iLocal) * kInvPi };
  const ColorRGB BRDF{ m_albedo * kInvPi };

  if (!isFinite(PDF) || PDF <= Float{} || !isFinite(BRDF)) return std::nullopt;

  return BSDFSample{ unitW_iLocal, std::nullopt, BRDF, PDF, BxDFType::Reflection | BxDFType::Diffuse };
}

[[nodiscard]] Float LambertianBxDF::PDF([[maybe_unused]] const Vec3f& unitW_oLocal, [[maybe_unused]] const Vec3f& unitW_iLocal) const noexcept
{
  if (!isFinite(unitW_oLocal) || !isFinite(unitW_iLocal)) return Float{};
  if (!sameHemisphereLocal(unitW_oLocal, unitW_iLocal)) return Float{};

  return absCosineThetaLocal(unitW_iLocal) * kInvPi;
}

[[nodiscard]] BxDFType LambertianBxDF::type() const noexcept
{
  return BxDFType::Reflection | BxDFType::Diffuse;
}