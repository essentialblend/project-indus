export module indus.shading.coated_diffuse;

import indus.core.types;
import indus.core.colorrgb;

import indus.shading.material;

export class MCoatedDiffuse final : public Material
{
public:
  MCoatedDiffuse(const ColorRGB& baseReflectance, Float coatEta, Float coatRoughness) noexcept;

  [[nodiscard]] MaterialType getMaterialType() const noexcept override;
  [[nodiscard]] ColorRGB getReflectance() const noexcept override;

  [[nodiscard]] Float getCoatEta() const noexcept;
  [[nodiscard]] Float getCoatRoughness() const noexcept;

private:
  ColorRGB m_baseReflectance{};
  Float m_coatEta{ 1.5 };
  Float m_coatRoughness{};
};

MCoatedDiffuse::MCoatedDiffuse(const ColorRGB& baseReflectance, Float coatEta, Float coatRoughness) noexcept : 
  m_baseReflectance{ baseReflectance }, m_coatEta{ coatEta }, m_coatRoughness{ coatRoughness } {}

MaterialType MCoatedDiffuse::getMaterialType() const noexcept
{
  return MaterialType::CoatedDiffuse;
}

ColorRGB MCoatedDiffuse::getReflectance() const noexcept
{
  return m_baseReflectance;
}

Float MCoatedDiffuse::getCoatEta() const noexcept
{
  return m_coatEta;
}

Float MCoatedDiffuse::getCoatRoughness() const noexcept
{
  return m_coatRoughness;
}
