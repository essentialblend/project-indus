export module indus.shading.bxdf_factory;

import indus.shading.bsdf;
import indus.shading.dielectric_bxdf;
import indus.shading.lambertian_bxdf;
import indus.shading.diffuse;
import indus.shading.dielectric;
import indus.shading.material;

import indus.geom.surface_interaction;

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
      const auto& d{ static_cast<const MDielectric&>(mat) };
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