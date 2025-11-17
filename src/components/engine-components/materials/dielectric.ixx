export module dielectric;

import <cassert>;

import std;
import bxdf;
import bsdf_new;
import material;
import dielectricbxdf;
import colorrgb;
import types;
import surfaceinteraction;

export class MDielectric final : public Material 
{
public:
  MDielectric(const ColorRGB&, const ColorRGB&, Float, Float) noexcept;

  virtual MaterialType getMaterialType() const noexcept override;
  virtual ColorRGB getReflectance() const noexcept override;
  ColorRGB getTransmittance() const noexcept;
  std::pair<Float, Float> getEtaCoefficients() const noexcept;

private:
  ColorRGB m_reflectance{};
  ColorRGB m_transmittance{};
  Float m_etaI{};
  Float m_etaT{};
};

MDielectric::MDielectric(const ColorRGB& reflectance, const ColorRGB& transmittance, Float etaI, Float etaT) noexcept : m_reflectance{ reflectance }, m_transmittance{ transmittance }, m_etaI{ etaI }, m_etaT{ etaT } {}

MaterialType MDielectric::getMaterialType() const noexcept
{
  return MaterialType::Glass;
}

ColorRGB MDielectric::getReflectance() const noexcept
{
  return m_reflectance;
}

ColorRGB MDielectric::getTransmittance() const noexcept
{
  return m_transmittance;
}

std::pair<Float, Float> MDielectric::getEtaCoefficients() const noexcept
{
  return { m_etaI, m_etaT };
}
