export module indus.shading.bxdf_factory;

import indus.shading.material;
import indus.shading.bsdf;
import indus.shading.dielectric_bxdf;
import indus.shading.lambertian_bxdf;
import indus.shading.coateddiffuse_bxdf;
import indus.shading.diffuse;
import indus.shading.dielectric;
import indus.shading.coated_diffuse;

import indus.geom.surface_interaction;

export
{
  BSDF computeBSDF(const Material& mat, const SurfaceInteraction& si)
  {
    switch (mat.getMaterialType())
    {
    case MaterialType::Diffuse:
    {
      const auto& material{ static_cast<const Diffuse&>(mat) };
      BSDF bsdf{ si.getShadingBasis() };
      bsdf.setBxDF(std::make_unique<LambertianBxDF>(material.getReflectance()));

      return bsdf;
    }

    case MaterialType::Glass:
    {
      const auto& material{ static_cast<const MDielectric&>(mat) };
      const auto& [etaI, etaT] { material.getEtaCoefficients() };
      BSDF bsdf{ si.getShadingBasis() };

      bsdf.setBxDF(std::make_unique<DielectricBxDF>(material.getReflectance(), material.getTransmittance(), etaI, etaT));

      return bsdf;
    }

    case MaterialType::CoatedDiffuse:
    {
      const auto& cd{ static_cast<const MCoatedDiffuse&>(mat) };
      BSDF bsdf{ si.getShadingBasis() };

      bsdf.setBxDF(std::make_unique<CoatedDiffuseBxDF>(cd.getReflectance(), cd.getCoatEta(), cd.getCoatRoughness()));

      return bsdf;
    }
    }

    // Degenerate fallback
    return {};
  }
};