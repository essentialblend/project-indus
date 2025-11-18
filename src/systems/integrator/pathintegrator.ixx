export module indus.integrator.path;

import indus.core.math.algebra.iv;

import indus.shading.bsdf;
import indus.shading.bxdf_factory;

import indus.stats.accumulator;

import indus.integrator.ray;

// TODOs: RDs, Spectral, Denoising, NEE/MIS, emission/lights, ScratchBuffers
export class PathIntegrator final : public RayIntegrator 
{
public:
  PathIntegrator(CameraBase&, Sampler&, Idx, bool useRR = true) noexcept;

  [[nodiscard]] virtual std::string toString() const override;

protected:
  ColorRGB Li(const Ray&, const Scene&, /*int depth,*/ Sampler&) override;

private:
  Idx m_maxDepth{};
  bool m_useRR{ true };

  ColorRGB getBackgroundGradient(const Ray& inputRay);
};

PathIntegrator::PathIntegrator(CameraBase& camera, Sampler& sampler, Idx maxDepth, bool useRR) noexcept : RayIntegrator{ camera, sampler }, m_maxDepth{ maxDepth }, m_useRR{ useRR } {}

std::string PathIntegrator::toString() const
{
  return "path";
}

ColorRGB PathIntegrator::Li(const Ray& inputRay, const Scene& scene, Sampler& sampler)
{
  ColorRGB L{ 0 }, beta{ 1 }; Ray ray{ inputRay };

  for (Idx bounce{}; bounce < m_maxDepth; ++bounce)
  {
    auto shapeIntersection{ scene.intersect(ray) };

    if (!shapeIntersection)
    {
      L += beta * getBackgroundGradient(ray);
      break;
    }

    const SurfaceInteraction& si{ shapeIntersection->interaction };

    auto mat{ si.getMaterial() };
    if (!mat) break;

    BSDF bsdf{ computeBSDF(*mat, si) };

    const Float RRUSample{ sampler.get1D() };
    const Point2f BSDFUSample{ sampler.get2D() };

    auto BSDFSample{ bsdf.sample(-ray.getDirection(), BSDFUSample) };

    if (!BSDFSample || !BSDFSample->unitW_iWorld || !(BSDFSample->PDF > Float{})) break;

    const Vec3f wi{ *BSDFSample->unitW_iWorld };
    const Float incidentCosineTheta{ absDot(wi, si.getShadingBasis().getNormal()) };
    beta *= BSDFSample->BRDF * (incidentCosineTheta / BSDFSample->PDF);

    if (m_useRR && bounce >= 5)
    {
      const Float q{ Float{ std::min(Float{ 0.95 }, std::max({ beta[0], beta[1], beta[2] })) } };

      if (RRUSample > q) break;

      beta *= (Float{ 1 } / q);
    }

    ray = si.spawnRay(wi);
    StatsAccumulator::recordIndirectRay();
  }
  return L;
}

ColorRGB PathIntegrator::getBackgroundGradient(const Ray& inputRay)
{
  const ColorRGB gradientColorVec{ Float{ 0.55 }, Float{ 0.7 }, Float{ 1.0 } };

  const Vec3f inputRayDir{ normalize(inputRay.getDirection()) };
  const Float lerpFactor{ Float{ 0.75 } * (inputRayDir[1] + Float{ 1.0 }) };
  const ColorRGB returnedColor{ (Float{ 1.0 } - lerpFactor) * ColorRGB{ 1.0 } + (lerpFactor * gradientColorVec) };

  return returnedColor;
}