export module bxdf;

import vec3;
import std;
import core_constructs;

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

  [[nodiscard]] virtual auto evaluate([[maybe_unused]] const Vec3& unitLocalW_o, [[maybe_unused]] const Vec3& unitLocalW_i) const noexcept -> ColorRGB = 0;
  
  [[nodiscard]] virtual auto sample([[maybe_unused]] const Vec3& unitLocalW_o, const Sample2D& uniformSample) const -> std::tuple<Vec3, double, ColorRGB, BxDFType> = 0;
  
  [[nodiscard]] virtual auto PDF([[maybe_unused]] const Vec3& unitLocalW_o, [[maybe_unused]] const Vec3& unitLocalW_i) const noexcept -> double = 0;
  
  [[nodiscard]] virtual auto type() const noexcept -> BxDFType = 0;

private:
  BxDFType m_type{};
};