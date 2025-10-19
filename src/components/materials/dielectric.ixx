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

  virtual BSDF getBSDF(const SurfaceInteraction& si) const override;

  virtual MaterialType getMaterialType() const noexcept override;

private:
  ColorRGB m_reflectance{};
  ColorRGB m_transmittance{};
  Float m_etaI{};
  Float m_etaT{};
};

MDielectric::MDielectric(const ColorRGB& reflectance, const ColorRGB& transmittance, Float etaI, Float etaT) noexcept : m_reflectance{ reflectance }, m_transmittance{ transmittance }, m_etaI{ etaI }, m_etaT{ etaT } {}

BSDF MDielectric::getBSDF(const SurfaceInteraction& si) const 
{
  BSDF bsdf{ si.getShadingBasis() };

  bsdf.setBxDF(std::make_unique<DielectricBxDF>(m_reflectance, m_transmittance, m_etaI, m_etaT));

  return bsdf;
}

MaterialType MDielectric::getMaterialType() const noexcept
{
  return MaterialType::Glass;
}