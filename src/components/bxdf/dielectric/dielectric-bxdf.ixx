export module dielectricbxdf;

import std;
import bxdf;
import vec3;
import fresneldielectric;

export class DielectricBxDF final : public BxDF {
public:
  DielectricBxDF(const ColorRGB& reflectance,
    const ColorRGB& transmittance,
    double etaI,
    double etaT) noexcept;


  [[nodiscard]] auto evaluate(const Vec3& unitLocalW_o,
    const Vec3& unitLocalW_i) const noexcept -> ColorRGB override;


  [[nodiscard]] auto sample(const Vec3& unitLocalW_o,
    const Sample2D& s) const
    -> std::tuple<Vec3, double, ColorRGB, BxDFType> override;


  [[nodiscard]] auto PDF(const Vec3&, const Vec3&) const noexcept -> double override;


  [[nodiscard]] auto type() const noexcept -> BxDFType override;


private:
  ColorRGB m_reflectance{};
  ColorRGB m_transmittance{};
  double m_etaI{};
  double m_etaT{};
};