export module indus.geom.sphere;

import std;

import indus.core.types;
import indus.core.math.fp.ii;
import indus.core.math.trig.iii;
import indus.core.math.algebra.iv;
import indus.core.math.float_constants;
import indus.core.math.interval;
import indus.core.geom.transform;

import indus.geom.shape;
import indus.geom.intersection_constructs;
import indus.geom.surfaceinteraction;

export class Sphere final : public Shape
{
public:
  Sphere(const Transform4f& renderFromObject, const Transform4f& objectFromRender, bool reverseOrientation, Float radius, Float zMin, Float zMax, Float phiMax) noexcept;

  [[nodiscard]] std::optional<ShapeIntersection> intersect(const Ray& ray) const override;

  [[nodiscard]] bool intersectP(const Ray& ray) const override;

  [[nodiscard]] Bounds3f getBounds() const noexcept override;
  [[nodiscard]] Float getRadius() const noexcept;

private:
  Transform4f m_renderFromObject{};
  Transform4f m_objectFromRender{};
  bool m_reverseOrientation{};
  bool m_swapsHandedness{};

  Float m_radius{};
  Float m_zMin{}; Float m_zMax{};
  Float m_phiMaxRadians{};

  [[nodiscard]] std::optional<QuadricIntersection> basicIntersect(const Ray& ray) const;

  [[nodiscard]] SurfaceInteraction interactionFromIntersection(const QuadricIntersection& quadricIntersection, const Vec3f& w_oWorld, Float timeSec) const override;
};

Sphere::Sphere(const Transform4f& renderFromObject, const Transform4f& objectFromRender, bool reverse, Float radius, Float zMin, Float zMax, Float phiMax) noexcept : m_renderFromObject{ renderFromObject }, m_objectFromRender{ objectFromRender }, m_reverseOrientation{ reverse }, m_swapsHandedness{ renderFromObject.swapsHandedness() }, m_radius{ radius }, m_zMin{ std::min(zMin, zMax) }, m_zMax{ std::max(zMin, zMax) }, m_phiMaxRadians{ degreesToRadians(clamp(phiMax, Float{ 0 }, Float{ 360 })) } {}

Bounds3f Sphere::getBounds() const noexcept 
{
  const Point3f min{ -m_radius, -m_radius, -m_radius };
  const Point3f max{ m_radius,  m_radius,  m_radius };

  Bounds3f bounds{};

  for (Int i{}; i < 8; ++i)
  {
    const Point3f pObj
    {
      (i & 1) ? max[0] : min[0],
      (i & 2) ? max[1] : min[1],
      (i & 4) ? max[2] : min[2]
    };
    const Point3f pRen{ m_renderFromObject(pObj) };
    
    bounds = Bounds3f::getUnion(bounds, pRen);
  }

  return bounds;
}

Float Sphere::getRadius() const noexcept
{
  return m_radius;
}

bool Sphere::intersectP(const Ray& r) const 
{
  return basicIntersect(r).has_value();
}

std::optional<QuadricIntersection> Sphere::basicIntersect(const Ray& ray) const
{
  const Float tMaxObject{ ray.getTMax() };

  const Point3fi originI{ m_objectFromRender(Point3fi{ ray.getOrigin() }) };
  const Vec3fi dirI{ m_objectFromRender(Vec3fi{ ray.getDirection() }) };

  const Intervalf a{ computeDot(dirI, dirI) };
  const Intervalf b{ Intervalf{ 2 } * computeDot(dirI, originI) };
  const Intervalf c{ computeDot(originI, originI) - Intervalf{ m_radius } * Intervalf{ m_radius } };

  const Intervalf halfInvA{ Intervalf{ 1 } / (Intervalf{ 2 } * a) };
  const Intervalf f{ b * halfInvA };
  const Vec3fi closest{ originI[0] - (f * dirI[0]), originI[1] - (f * dirI[1]), originI[2] - (f * dirI[2]) };
  const Intervalf dist{ sqrtI(computeDot(closest, closest)) };

  const Intervalf discr{ Intervalf{ 4 } * a * (Intervalf{ m_radius } + dist) * (Intervalf{ m_radius } - dist) };
  
  if (discr.getLower() < Float{}) return std::nullopt;

  const Intervalf q{ (b.getMid() < Float{}) ? Intervalf{ Float{ -0.5 } } * (b - sqrtI(discr)) : Intervalf{ Float{ -0.5 } } * (b + sqrtI(discr)) };

  Intervalf tFrontI{ q / a }; Intervalf tBackI{ c / q };
  if (tFrontI.getLower() > tBackI.getLower()) std::swap(tFrontI, tBackI);

  if (tFrontI.getUpper() > tMaxObject || tBackI.getLower() <= 0) return std::nullopt;
  
  auto tShapeHit{ tFrontI };
  bool usingFront{ true };

  if (tShapeHit.getLower() <= Float{})
  { 
    tShapeHit = tBackI; 
    usingFront = false; 
    if (tShapeHit.getUpper() > tMaxObject) return std::nullopt; 
  }

  Float tHit{ tShapeHit.getMid() };

  Point3f pHitObj{};
  Float phi{};

  for (Int attempt{}; attempt < 2; ++attempt)
  {
    pHitObj = Point3f{ originI } + (Vec3f{ dirI } * tHit);

    const Float invLen{ Float{ 1 } / euclideanLength(pHitObj) };
    
    pHitObj *= (m_radius * invLen);

    if (isZero(pHitObj[0]) && isZero(pHitObj[1])) pHitObj[0] = kSafeNormalizeLen<Float> * m_radius;

    phi = std::atan2(pHitObj[1], pHitObj[0]);
    
    if (phi < Float{}) phi += Float{ 2 } * kPi;

    const bool zClipped{ ((m_zMin > -m_radius) && (pHitObj[2] < m_zMin)) || ((m_zMax < m_radius) && (pHitObj[2] > m_zMax)) };

    if (zClipped || (phi > m_phiMaxRadians)) 
    {
      if (!usingFront) return std::nullopt;
      
      if (tBackI.getUpper() > tMaxObject) return std::nullopt;
      
      tHit = tBackI.getMid(); 
      usingFront = false;
    }
  }

  return QuadricIntersection{ tHit, pHitObj, phi };
}

SurfaceInteraction Sphere::interactionFromIntersection(const QuadricIntersection& quadricIntersection, [[maybe_unused]] const Vec3f& w_oWorld, Float timeSec) const
{
  const Point3f pObj{ quadricIntersection.pHitPos };
  const Point3f pRen{ m_renderFromObject(pObj) };

  const Float phi{ quadricIntersection.phi };
  const Float u{ (m_phiMaxRadians > 0) ? (phi / m_phiMaxRadians) : Float{} };

  const Float theta{ safeACos(pObj[2] / m_radius) };
  const Float thetaMin{ safeACos(m_zMax / m_radius) };
  const Float thetaMax{ safeACos(m_zMin / m_radius) };
  const Float v{ (thetaMax != thetaMin) ? ((theta - thetaMin) / (thetaMax - thetaMin)) : Float{} };

  Normal3f nObj{ normalize(Normal3f{ pObj[0] / m_radius, pObj[1] / m_radius, pObj[2] / m_radius }) };
  
  if (m_reverseOrientation ^ m_swapsHandedness) nObj = Normal3f{ -nObj[0], -nObj[1], -nObj[2] };

  Normal3f nRen{ normalize(m_renderFromObject(nObj)) };

  const Vec3f pErr{ gamma<Float>(5) * std::abs(pRen[0]), gamma<Float>(5) * std::abs(pRen[1]), gamma<Float>(5) * std::abs(pRen[2]) };

  SurfaceInteraction si{ pRen, timeSec, quadricIntersection.tHit, pErr, nRen };
  
  si.setUV(Point2f{ u, v });
  
  return si;
}

std::optional<ShapeIntersection> Sphere::intersect(const Ray& ray) const 
{
  const auto quadricIntersection{ basicIntersect(ray) };
  
  if (!quadricIntersection) return std::nullopt;

  const auto surfaceInteraction{ interactionFromIntersection(*quadricIntersection, -ray.getDirection(), ray.getTimeSeconds()) };

  return ShapeIntersection{ std::move(surfaceInteraction), quadricIntersection->tHit };
}
