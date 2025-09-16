export module glass;

import <cassert>;

import std;
import hit_record;
import bxdf;
import bsdf;
import material;
import dielectricbxdf;
import colorrgb;
import types;

export class MGlass final : public IMaterial 
{
public:
  MGlass(const ColorRGB&, const ColorRGB&, Float, Float) noexcept;

  void computeScatteringFunctions(HitRecord& hitRec) const override;

  virtual MaterialType getMaterialType() const noexcept override;

private:
  ColorRGB m_reflectance{};
  ColorRGB m_transmittance{};
  Float m_etaI{};
  Float m_etaT{};
};

MGlass::MGlass(const ColorRGB& reflectance, const ColorRGB& transmittance, Float etaI, Float etaT) noexcept : m_reflectance{ reflectance }, m_transmittance{ transmittance }, m_etaI{ etaI }, m_etaT{ etaT } {}

void MGlass::computeScatteringFunctions(HitRecord& hitRec) const
{
  hitRec.surfaceBSDF = std::make_unique<BSDF>(hitRec.shadingBasis);
  hitRec.surfaceBSDF->clearBxDFs();

  Float ei{ hitRec.hitFrontFace ? m_etaI : m_etaT };
  Float et{ hitRec.hitFrontFace ? m_etaT : m_etaI };

  hitRec.surfaceBSDF->addBxDF(std::make_unique<DielectricBxDF>(m_reflectance, m_transmittance, ei, et));
}

MaterialType MGlass::getMaterialType() const noexcept
{
  return MaterialType::Glass;
}