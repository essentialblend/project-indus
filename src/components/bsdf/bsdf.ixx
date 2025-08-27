export module bsdf;

import std;
import vec3;
import bxdf;
import core_constructs;

export class BSDF final 
{
public:
  explicit BSDF() noexcept = default;
  BSDF(const OrthonormalBasis& basis) noexcept;
  BSDF(const BSDF&) = delete;
  BSDF& operator=(const BSDF&) = delete;

  BSDF(BSDF&&) noexcept = default;
  BSDF& operator=(BSDF&&) noexcept = default;

  [[nodiscard]] auto evaluate(const Vec3& w_o, const Vec3& w_i) const noexcept -> ColorRGB;
  [[nodiscard]] auto sample(const Vec3& unitW_o, const Sample2D& uniformSample) const -> std::tuple<Vec3, double, ColorRGB, BxDFType>;
  [[nodiscard]] auto PDF(const Vec3& unitW_o, const Vec3& unitW_i) const noexcept -> double;
  
  [[nodiscard]] auto worldToLocal(const Vec3& vec) const noexcept -> Vec3;
  [[nodiscard]] auto localToWorld(const Vec3& vec) const noexcept -> Vec3;

  void addBxDF(std::unique_ptr<BxDF> bxdf) noexcept;
  void clearBxDFs() noexcept;

private:
  OrthonormalBasis m_basis{};
  std::vector<std::unique_ptr<BxDF>> m_bxdfs{};
};
