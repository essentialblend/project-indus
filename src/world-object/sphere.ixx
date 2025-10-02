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
	explicit WOSphere(const Point3f& center, Float radius, std::shared_ptr<IMaterial> material) noexcept;

	bool checkHit(const Ray& incidentRay, Float tMax, HitRecord& hitRec) const override;

private:
	Point3f m_sphereCenter{};
	Float m_sphereRadius{};
	std::shared_ptr<IMaterial> m_sphereMaterial{};
};

WOSphere::WOSphere(const Point3f& center, Float radius, std::shared_ptr<IMaterial> material) noexcept : m_sphereCenter{ center }, m_sphereRadius{ static_cast<Float>(std::fmax(0, radius)) }, m_sphereMaterial{ material } {}

bool WOSphere::checkHit(const Ray& incidentRay, Float tMax, HitRecord& hitRec) const
{
  // Setup the quadratic, oc = o - c, and square of a vector norm is dot(v,v) and we obtain the coefficients
  const Vec3f oc{ incidentRay.getOrigin() - m_sphereCenter };
  const Float a{ euclideanLengthSq(incidentRay.getDirection()) };
  const Float b{ Float(2) * computeDot(incidentRay.getDirection(), oc) };
  const Float c{ differenceOfProducts(euclideanLengthSq(oc), Float(1), m_sphereRadius, m_sphereRadius) };
  
  const auto roots{ evaluateQuadratic(a, b, c) };
  
  if (!roots) return false;
  
  const auto& [t0, t1] = *roots;
  
  Float root{ t0 };
  
  if (!(root > Float{} && root < tMax)) 
  { 
    root = t1; 
    if (!(root > Float{} && root < tMax)) return false; 
  }

  const Point3f pRaw{ incidentRay.getPointAt(root) };
  const Vec3f fromCenter{ pRaw - m_sphereCenter };
  const Float invLen{ Float(1) / euclideanLength(fromCenter) };
  const Vec3f pLocal{ fromCenter * (m_sphereRadius * invLen) };
  
  // Use the geometry of the sphere/primitive to get the geometric normal   
  const Point3f pHat{ m_sphereCenter + pLocal };
  
  // Calculate the error to account for compounding floating point precision loss
  const Vec3f pError{ gamma(5) * Vec3f{ std::abs(pLocal[0]), std::abs(pLocal[1]), std::abs(pLocal[2]) } };

  hitRec.root = root;
  hitRec.hitPoint = pHat;
  hitRec.pError = pError;
  hitRec.geometricNormal = Normal3f{ fromCenter * invLen };
  hitRec.setFaceNormal(incidentRay, hitRec.geometricNormal);
  hitRec.hitMaterial = m_sphereMaterial;
  
  return true;
}