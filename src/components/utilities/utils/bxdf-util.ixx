export module bxdfutil;

import bsdf_new;
import dielectricbxdf;
import lambertianBxDF;
import diffuse;
import dielectric;
import material;
import surfaceinteraction;

export
{
  BSDF computeBSDF(const Material& mat, const SurfaceInteraction& si)
  {
    switch (mat.getMaterialType())
    {
    case MaterialType::Diffuse:
    {
      const auto& d{ static_cast<const Diffuse&>(mat) };
      BSDF bsdf{ si.getShadingBasis() };
      bsdf.setBxDF(std::make_unique<LambertianBxDF>(d.getReflectance()));
      
      return bsdf;
    }

    case MaterialType::Glass:
    {
      const auto& d = static_cast<const MDielectric&>(mat);
      const auto& [etaI, etaT] { d.getEtaCoefficients() };
      BSDF bsdf{ si.getShadingBasis() };
      
      bsdf.setBxDF(std::make_unique<DielectricBxDF>(d.getReflectance(), d.getTransmittance(), etaI, etaT));
      
      return bsdf;
    }
    }

    // Degenerate fallback
    return {};
  }
};