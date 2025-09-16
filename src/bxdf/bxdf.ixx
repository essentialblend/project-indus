export module bxdf;

import vector;
import std;
import colorrgb;
import constructs;
import point;
import types;

export enum class BxDFType : std::uint8_t
{
  Reflection   = 1 << 0,
  Transmission = 1 << 1,
  Diffuse      = 1 << 2,
  Glossy       = 1 << 3, 
  Specular     = 1 << 4
};

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

  [[nodiscard]] virtual ColorRGB evaluate([[maybe_unused]] const Vec3f& unitLocalW_o, [[maybe_unused]] const Vec3f& unitLocalW_i) const noexcept = 0;
  
  [[nodiscard]] virtual std::tuple<Vec3f, Float, ColorRGB, BxDFType> sample([[maybe_unused]] const Vec3f& unitLocalW_o, const Point2f& uniformSample) const = 0;
  
  [[nodiscard]] virtual Float PDF([[maybe_unused]] const Vec3f& unitLocalW_o, [[maybe_unused]] const Vec3f& unitLocalW_i) const noexcept = 0;
  
  [[nodiscard]] virtual BxDFType type() const noexcept = 0;

private:
  BxDFType m_type{};
};