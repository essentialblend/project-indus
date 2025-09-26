export module pathintegrator;

import std;

import <cassert>;

import vector;
import constructs;
import camerabase;
import sampler;
import rayintegrator;
import world_object;
import ray;
import types;
import colorrgb;
import core_diag;
import hit_record;
import material;
import core_sampling_util;

import <cassert>;

// TODOs: RDs, Spectral, Denoising, NEE/MIS, emission/lights, ScratchBuffers

export class PathIntegrator final : public RayIntegrator 
{
public:
  PathIntegrator(CameraBase&, Sampler&, Idx, bool useRR = true) noexcept;

protected:
  ColorRGB Li(const Ray&, const WorldObject&, /*int depth,*/ Sampler&) override;

private:
  Idx m_maxDepth{};
  bool m_useRR{ true };

  ColorRGB getBackgroundGradient(const Ray& inputRay);
};

PathIntegrator::PathIntegrator(CameraBase& camera, Sampler& sampler, Idx maxDepth, bool useRR) noexcept : RayIntegrator{ camera, sampler }, m_maxDepth{ maxDepth }, m_useRR{ useRR } {}

ColorRGB PathIntegrator::Li(const Ray& inputRay, const WorldObject& world, Sampler& sampler)
{
  ColorRGB L{ 0 };
  ColorRGB beta{ 1 };
  Ray ray{ inputRay };

  for (Idx bounce{}; bounce < m_maxDepth; ++bounce)
  {
    const Float RRSample{ sampler.get1D() };
    const Point2f BSDFSample{ sampler.get2D() };

    HitRecord hit{};
    
    if (!world.checkHit(ray, std::numeric_limits<Float>::infinity(), hit))
    {
      L += beta * getBackgroundGradient(ray);
      break;
    }

    hit.hitMaterial->computeScatteringFunctions(hit);

    if (!hit.surfaceBSDF) break;

    const auto sampleBSDF{ hit.surfaceBSDF->sample(-ray.getDirection(), BSDFSample) };
    if (!sampleBSDF || !sampleBSDF->unitW_iWorld) break;

    const Vec3f unitW_iWorld{ *sampleBSDF->unitW_iWorld };
    const Float PDFVal{ sampleBSDF->PDF };
    const ColorRGB BRDFVal{ sampleBSDF->BRDF };
    //const BxDFType flags{ sampleBSDF->flags };

    const Float cosineTheta{ std::abs(computeDot(unitW_iWorld, hit.shadingBasis.getNormal())) };
    
    if (!(PDFVal > 0.0)) break;

    beta *= BRDFVal * (cosineTheta / PDFVal);

    if (m_useRR && bounce >= 5)
    {
      const Float q{ Float(std::min(Float(0.95), std::max({ beta[0], beta[1], beta[2] }))) };
      if (RRSample > q) break;
      beta *= (Float(1.0) / q);   
    }

    ray = hit.spawnRay(unitW_iWorld);
  }

  return L;
}

ColorRGB PathIntegrator::getBackgroundGradient(const Ray& inputRay)
{
  const ColorRGB gradientColorVec{ Float(0.55), Float(0.7), Float(1.0) };

  const Vec3f inputRayDir{ normalize(inputRay.getDirection()) };
  const Float lerpFactor{ Float(0.75 * (inputRayDir[1] + 1.0)) };
  const ColorRGB returnedColor{ (Float(1.0 - lerpFactor) * ColorRGB{ 1.0 }) + (lerpFactor * gradientColorVec) };

  return returnedColor;
}