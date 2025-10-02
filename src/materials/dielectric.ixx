export module dielectric;

import <cassert>;

import std;
import hit_record;
import bxdf;
import bsdf_new;
import material;
import dielectricbxdf;
import colorrgb;
import types;

export class MDielectric final : public IMaterial 
{
public:
  MDielectric(const ColorRGB&, const ColorRGB&, Float, Float) noexcept;

  void computeScatteringFunctions(HitRecord& hitRec) const override;

  virtual MaterialType getMaterialType() const noexcept override;

private:
  ColorRGB m_reflectance{};
  ColorRGB m_transmittance{};
  Float m_etaI{};
  Float m_etaT{};
};

MDielectric::MDielectric(const ColorRGB& reflectance, const ColorRGB& transmittance, Float etaI, Float etaT) noexcept : m_reflectance{ reflectance }, m_transmittance{ transmittance }, m_etaI{ etaI }, m_etaT{ etaT } {}

void MDielectric::computeScatteringFunctions(HitRecord& hitRec) const
{
  hitRec.surfaceBSDF = std::make_unique<BSDF>(hitRec.shadingBasis);

  // Pass fixed media: outside: etaI (air), inside: etaT (glass)
  hitRec.surfaceBSDF->setBxDF(std::make_unique<DielectricBxDF>(m_reflectance, m_transmittance, m_etaI, m_etaT));
}

MaterialType MDielectric::getMaterialType() const noexcept
{
  return MaterialType::Glass;
}