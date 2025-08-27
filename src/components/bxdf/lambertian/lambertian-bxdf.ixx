export module lambertianBxDF;

import std;
import vec3;
import bxdf;

export class LambertianBxDF final : public BxDF
{
public:
  explicit LambertianBxDF(const ColorRGB& albedo) noexcept;

  [[nodiscard]] virtual auto evaluate([[maybe_unused]] const Vec3& localW_o, [[maybe_unused]] const Vec3& localW_i) const noexcept -> ColorRGB override;
  
  [[nodiscard]] virtual auto sample([[maybe_unused]] const Vec3& unitLocalW_o, const Sample2D& uniformSample) const -> std::tuple<Vec3, double, ColorRGB, BxDFType> override;
  
  [[nodiscard]] virtual auto PDF([[maybe_unused]] const Vec3& unitLocalW_o, [[maybe_unused]] const Vec3& unitLocalW_i) const noexcept -> double override;
  
  [[nodiscard]] virtual auto type() const noexcept -> BxDFType override;

private:
  ColorRGB m_albedo{};
};