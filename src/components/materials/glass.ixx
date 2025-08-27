export module glass;

import <cassert>;

import std;
import vec3;
import ray;
import hit_record;
import bxdf;
import bsdf;
import material;
import dielectricbxdf;

export class MGlass final : public IMaterial 
{
public:
  MGlass(const ColorRGB& reflectance, const ColorRGB& transmittance, double etaI, double etaT) noexcept : m_reflectance{ reflectance }, m_transmittance{ transmittance }, m_etaI{ etaI }, m_etaT{ etaT } {}

  void computeScatteringFunctions(HitRecord& hitRec) const override 
  {
    hitRec.surfaceBSDF = std::make_unique<BSDF>(hitRec.shadingBasis);
    hitRec.surfaceBSDF->clearBxDFs();

    double ei{ hitRec.hitFrontFace ? m_etaI : m_etaT };
    double et{ hitRec.hitFrontFace ? m_etaT : m_etaI };

    hitRec.surfaceBSDF->addBxDF(std::make_unique<DielectricBxDF>(m_reflectance, m_transmittance, ei, et));
  }

  virtual MaterialType getMaterialType() const noexcept override
  {
    return MaterialType::Glass;
  }


private:
  ColorRGB m_reflectance;
  ColorRGB m_transmittance;
  double m_etaI;
  double m_etaT;
};