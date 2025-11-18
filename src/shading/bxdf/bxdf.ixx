export module indus.shading.bxdf;

import std;

import indus.core.colorrgb;
import indus.core.types;

import indus.sampling.constructs;

export constexpr auto operator|(BxDFType a, BxDFType b) noexcept -> BxDFType
{
  return static_cast<BxDFType>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

export constexpr auto operator&(BxDFType a, BxDFType b) noexcept -> BxDFType 
{
  return static_cast<BxDFType>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
}

export constexpr bool isSpecularBxDF(BxDFType type) noexcept
{
  return static_cast<bool>(type & BxDFType::Specular);
}

export class BxDF
{
public:
  explicit BxDF(BxDFType type) noexcept : m_type{ type } {}
  virtual ~BxDF() = default;

  [[nodiscard]] virtual ColorRGB evaluate([[maybe_unused]] const Vec3f& unitW_oLocal, [[maybe_unused]] const Vec3f& unitW_iLocal) const noexcept = 0;
  
  [[nodiscard]] virtual std::optional<BSDFSample> sample([[maybe_unused]] const Vec3f& unitW_oLocal, const Point2f& uniformSample) const = 0;
  
  [[nodiscard]] virtual Float PDF([[maybe_unused]] const Vec3f& unitW_oLocal, [[maybe_unused]] const Vec3f& unitW_iLocal) const noexcept = 0;
  
  [[nodiscard]] virtual BxDFType type() const noexcept = 0;

private:
  BxDFType m_type{};
};