export module sphere;

import <memory>;
import <cassert>;

import world_object;
import vector;
import material;
import point;
import types;
import hit_record;
import mathfp;
import mathalgebra;
import mathutil;

export class WOSphere : public WorldObject
{
public:
	explicit WOSphere() noexcept = default;
	explicit WOSphere(const Point3f& center, Float radius, std::shared_ptr<Material> material) noexcept;

  std::optional<SurfaceInteraction> checkHit(const Ray& incidentRay, Float tMax) const override;

private:
	Point3f m_sphereCenter{};
	Float m_sphereRadius{};
	std::shared_ptr<Material> m_sphereMaterial{};
};

WOSphere::WOSphere(const Point3f& center, Float radius, std::shared_ptr<Material> material) noexcept : m_sphereCenter{ center }, m_sphereRadius{ static_cast<Float>(std::fmax(0, radius)) }, m_sphereMaterial{ material } {}

std::optional<SurfaceInteraction> WOSphere::checkHit(const Ray& incidentRay, Float tMax) const 
{
  const Vec3f oc{ incidentRay.getOrigin() - m_sphereCenter };
  const Float a{ euclideanLengthSq(incidentRay.getDirection()) };
  const Float b{ Float(2) * computeDot(incidentRay.getDirection(), oc) };
  const Float c{ differenceOfProducts(euclideanLengthSq(oc), Float{ 1 }, m_sphereRadius, m_sphereRadius) };

  const auto roots{ evaluateQuadratic(a, b, c) };
  if (!roots) return std::nullopt;

  Float root{ roots->first };
  
  if (!(root > Float{} && root < tMax)) 
  { 
    root = roots->second; 
    if (!(root > Float{} && root < tMax)) return std::nullopt; 
  }

  const Point3f pRaw{ incidentRay.getPointAt(root) };
  const Vec3f fromCenter{ pRaw - m_sphereCenter };
  const Float invLen{ Float(1) / euclideanLength(fromCenter) };
  const Vec3f pLocal{ fromCenter * (m_sphereRadius * invLen) };
  const Point3f pHat{ m_sphereCenter + pLocal };
  const Vec3f pError{ gamma<Float>(5) * Vec3f { std::abs(pLocal[0]), std::abs(pLocal[1]), std::abs(pLocal[2]) } };
  const Normal3f gN{ fromCenter * invLen };

  SurfaceInteraction si{ pHat, incidentRay.getTimeSeconds(), root, pError, gN };
  si.setMaterial(m_sphereMaterial);
  si.orientToIncident(incidentRay);
  return si;
}