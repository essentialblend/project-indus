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
import interval;
import core_sampling_util;

export class WOSphere : public WorldObject
{
public:
	explicit WOSphere() noexcept = default;
	explicit WOSphere(const Point3f& center, Float radius, std::shared_ptr<IMaterial> material) noexcept;

	bool checkHit(const Ray& inputRay, Interval rayInterval, HitRecord& hitRec) const override;

private:
	Point3f m_sphereCenter{};
	Float m_sphereRadius{};
	std::shared_ptr<IMaterial> m_sphereMaterial{};
};

WOSphere::WOSphere(const Point3f& center, Float radius, std::shared_ptr<IMaterial> material) noexcept : m_sphereCenter(center), m_sphereRadius(radius), m_sphereMaterial{ material } {}

bool WOSphere::checkHit(const Ray& inputRay, Interval rayInterval, HitRecord& hitRec) const 
{
  Vec3f oc{ inputRay.getOrigin() - m_sphereCenter };

  Float a{ euclideanLengthSq(inputRay.getDirection()) };
  Float b{ Float(2.0) * computeDot(inputRay.getDirection(), oc) };
  Float c{ euclideanLengthSq(oc) - (m_sphereRadius * m_sphereRadius) };

  auto rootsOpt = evaluateQuadratic(a, b, c);
  if (!rootsOpt) return false;

  auto& [t0, t1] = *rootsOpt;
  Float root{ t0 };

  if (!rayInterval.isSurrounded(root)) 
  {
    root = t1;
    if (!rayInterval.isSurrounded(root)) return false;
  }

  hitRec.root = root;
  hitRec.hitPoint = inputRay.getPointAt(root);

  Vec3f unitOutNormal{ (hitRec.hitPoint - m_sphereCenter) / m_sphereRadius };
  Normal3f unitNormal{ unitOutNormal };
  hitRec.setFaceNormal(inputRay, unitNormal);
  hitRec.hitMaterial = m_sphereMaterial;

  return true;
}