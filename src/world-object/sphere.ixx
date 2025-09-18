export module sphere;

import <memory>;
import <cassert>;

import world_object;
import vector;
import material;
import point;
import types;
import core_util;
import hit_record;
import core_sampling_util;

export class WOSphere : public WorldObject
{
public:
	explicit WOSphere() noexcept = default;
	explicit WOSphere(const Point3f& center, Float radius, std::shared_ptr<IMaterial> material) noexcept;

	bool checkHit(const Ray& inputRay, Float tMax, HitRecord& hitRec) const override;

private:
	Point3f m_sphereCenter{};
	Float m_sphereRadius{};
	std::shared_ptr<IMaterial> m_sphereMaterial{};
};

WOSphere::WOSphere(const Point3f& center, Float radius, std::shared_ptr<IMaterial> material) noexcept : m_sphereCenter{ center }, m_sphereRadius{ static_cast<Float>(std::fmax(0, radius)) }, m_sphereMaterial{ material } {}

bool WOSphere::checkHit(const Ray& inputRay, Float tMax, HitRecord& hitRec) const 
{
  const Vec3f oc{ inputRay.getOrigin() - m_sphereCenter };
  const Float a{ euclideanLengthSq(inputRay.getDirection()) };
  const Float b{ Float(2) * computeDot(inputRay.getDirection(), oc) };
  const Float c{ euclideanLengthSq(oc) - (m_sphereRadius * m_sphereRadius) };
  
  const auto roots{ evaluateQuadratic(a, b, c) };
  
  if (!roots) return false;
  
  auto [t0, t1] = *roots;
  
  Float root{ t0 };
  
  if (!(root > Float(0) && root < tMax)) 
  { 
    root = t1; 

    if (!(root > Float(0) && root < tMax)) 
      return false; 
  }

  const Point3f pRaw{ inputRay.getPointAt(root) };
  const Vec3f fromCenter{ pRaw - m_sphereCenter };
  const Float invLen{ Float(1) / std::sqrt(euclideanLengthSq(fromCenter)) };
  const Point3f pHat{ m_sphereCenter + fromCenter * (m_sphereRadius * invLen) };
  const Normal3f geomNormal{ Normal3f{ fromCenter * invLen } };
  
  const Vec3f pLocal{ fromCenter * (m_sphereRadius * invLen) };
  const Vec3f pError{ gamma(5) * Vec3f{ std::abs(pLocal[0]), std::abs(pLocal[1]), std::abs(pLocal[2]) } };

  hitRec.root = root;
  hitRec.hitPoint = pHat;
  hitRec.pError = pError;
  hitRec.setFaceNormal(inputRay, geomNormal);
  hitRec.geometricNormal = geomNormal;
  hitRec.hitMaterial = m_sphereMaterial;
  
  return true;
}