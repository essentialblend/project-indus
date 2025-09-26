export module bsdf_new;

import std;

import bxdf;
import types;
import colorrgb;
import onb;
import constructs;
import core_diag;

export class BSDF final
{
public:
  constexpr BSDF() noexcept = default;
  constexpr explicit BSDF(const OrthonormalBasis& basis) noexcept;

  [[nodiscard]] ColorRGB evaluate(const Vec3f& unitW_oWorld, const Vec3f& unitW_iWorld) const noexcept;
  [[nodiscard]] std::optional<BSDFSample> sample(const Vec3f& unitW_oWorld, const Point2f& uniformSample) const noexcept;

  [[nodiscard]] Float PDF(const Vec3f& unitW_oWorld, const Vec3f& unitW_iWorld) const noexcept;

  constexpr void setBxDF(std::unique_ptr<BxDF> bxdf) noexcept;
  [[nodiscard]] constexpr const OrthonormalBasis& getBasis() const noexcept;

private:
  OrthonormalBasis m_basis{};
  std::unique_ptr<BxDF> m_bxdf{};
};

constexpr BSDF::BSDF(const OrthonormalBasis& basis) noexcept : m_basis{ basis }, m_bxdf{ nullptr } {}

[[nodiscard]] ColorRGB BSDF::evaluate(const Vec3f& unitW_oWorld, const Vec3f& unitW_iWorld) const noexcept
{
  if (!m_bxdf) return ColorRGB{};

  if (!isFinite(unitW_oWorld) || !isFinite(unitW_iWorld)) return ColorRGB{};

  const Vec3f w_oLocal{ m_basis.worldToLocal(unitW_oWorld) };
  const Vec3f unitW_iLocal{ m_basis.worldToLocal(unitW_iWorld) };

  if (!isFinite(w_oLocal) || !isFinite(unitW_iLocal)) return ColorRGB{};

  return m_bxdf->evaluate(w_oLocal, unitW_iLocal);
}

[[nodiscard]] Float BSDF::PDF(const Vec3f& unitW_oWorld, const Vec3f& unitW_iWorld) const noexcept
{
  if (!m_bxdf) return 0.0f;

  if (!isFinite(unitW_oWorld) || !isFinite(unitW_iWorld)) return Float{};

  const Vec3f w_oLocal{ m_basis.worldToLocal(unitW_oWorld) };
  const Vec3f unitW_iLocal{ m_basis.worldToLocal(unitW_iWorld) };

  if (!isFinite(w_oLocal) || !isFinite(unitW_iLocal)) return Float{};

  return m_bxdf->PDF(w_oLocal, unitW_iLocal);
}

[[nodiscard]] std::optional<BSDFSample> BSDF::sample(const Vec3f& unitW_oWorld, const Point2f& uniformSample) const noexcept
{
  if (!m_bxdf || !isFinite(unitW_oWorld)) return std::nullopt;

  const Vec3f w_oLocal{ m_basis.worldToLocal(unitW_oWorld) };
  if (!isFinite(w_oLocal)) return std::nullopt;

  const std::optional<BSDFSample> optSample{ m_bxdf->sample(w_oLocal, uniformSample) };
  if (!optSample || !optSample->unitW_iLocal) return std::nullopt;

  const Vec3f unitW_iLocal{ *optSample->unitW_iLocal };
  
  if (optSample->PDF <= 0 || !isFinite(unitW_iLocal) || !isFinite(optSample->PDF) || !isFinite(optSample->BRDF)) return std::nullopt;

  const Float PDF{ optSample->PDF };
  const ColorRGB BRDF{ optSample->BRDF };

  const Vec3f unitW_iWorld{ m_basis.localToWorld(unitW_iLocal) };
  if (!isFinite(unitW_iWorld)) return std::nullopt;

  return BSDFSample{ std::nullopt, unitW_iWorld, BRDF, PDF, optSample->flags };
}

constexpr const OrthonormalBasis& BSDF::getBasis() const noexcept
{
  return m_basis;
}

constexpr void BSDF::setBxDF(std::unique_ptr<BxDF> bxdf) noexcept
{
  m_bxdf = std::move(bxdf);
}