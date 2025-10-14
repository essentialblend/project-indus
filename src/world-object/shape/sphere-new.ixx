export module spherenew;

import std;
import shape;
import constructs;
import types;
import transform;
import surfaceinteraction;
import mathfp;
import mathtrig;
import mathconstants;
import mathalgebra;
import interval;

export class Sphere final : public Shape
{
public:
  Sphere(const Transform4f& renderFromObject, const Transform4f& objectFromRender, bool reverseOrientation, Float radius, Float zMin, Float zMax, Float phiMax) noexcept;

  [[nodiscard]] std::optional<ShapeIntersection> intersect(const Ray& ray, Float rayParamTMax) const override;

  [[nodiscard]] bool intersectP(const Ray& ray, Float rayParamTMax) const override;
  [[nodiscard]] std::optional<QuadricIntersection> intersectT(const Ray&, Float) const override;

  //[[nodiscard]] Bounds3f getBounds() const noexcept override;

private:
  Transform4f m_renderFromObject{};
  Transform4f m_objectFromRender{};
  bool m_reverseOrientation{};
  bool m_swapsHandedness{};

  Float m_radius{};
  Float m_zMin{}; Float m_zMax{};
  Float m_phiMax{};

  [[nodiscard]] std::optional<QuadricIntersection> basicIntersect(const Ray& ray, Float rayParamTMax) const;

  [[nodiscard]] SurfaceInteraction interactionFromIntersection(const QuadricIntersection& quadricIntersection, const Vec3f& w_oWorld, Float timeSec) const override;
};

Sphere::Sphere(const Transform4f& renderFromObject, const Transform4f& objectFromRender, bool reverse, Float radius, Float zMin, Float zMax, Float phiMax) noexcept : m_renderFromObject{ renderFromObject }, m_objectFromRender{ objectFromRender }, m_reverseOrientation{ reverse }, m_swapsHandedness{ renderFromObject.swapsHandedness() }, m_radius{ radius }, m_zMin{ std::min(zMin, zMax) }, m_zMax{ std::max(zMin, zMax) }, m_phiMax{ degreesToRadians(clamp(phiMax, Float{ 0 }, Float{ 360 })) } {}

//Bounds3f Sphere::getBounds() const noexcept 
//{
//  const Point3f pMin{ -m_radius, -m_radius,  m_zMin };
//  const Point3f pMax{ m_radius,  m_radius,  m_zMax };
//  return m_renderFromObject(Bounds3f{ pMin, pMax });
//}

bool Sphere::intersectP(const Ray& r, Float tMax) const 
{
  return basicIntersect(r, tMax).has_value();
}

std::optional<QuadricIntersection> Sphere::intersectT(const Ray& r, Float tMax) const
{
  return basicIntersect(r, tMax);
}

std::optional<QuadricIntersection> Sphere::basicIntersect(const Ray& ray, Float rayParamTMax) const
{
  // Get ray in render space, convert to interval, and transform to object space
  const auto& rayOriginPoint3f{ ray.getOrigin() }; 
  const auto& rayDirVec3f{ ray.getDirection() };
 
  Point3fi rayOriginPoint3fi{ m_objectFromRender(Point3fi{ rayOriginPoint3f }) };
  const Vec3fi rayDirVec3fi{ m_objectFromRender(Vec3fi{ rayDirVec3f }) };

  // Since render->object space causes ULP-drift, we offset the ray and adjust tMax to prevent overshooting
  Float rayParamTMaxObj{ rayParamTMax };
  {
    const Vec3f dObj{ rayDirVec3fi };
    const Float lengthSq{ computeDot(dObj, dObj) };

    if (lengthSq > Float{})
    {
      const Vec3f absd{ std::abs(dObj[0]), std::abs(dObj[1]), std::abs(dObj[2]) };
      const Float dt{ computeDot(absd, rayOriginPoint3fi.getError<Float>()) / lengthSq };
      const Vec3fi dI{ dObj };
      const Intervalf dtI{ dt };

      rayOriginPoint3fi = rayOriginPoint3fi + (dI * dtI);
      rayParamTMaxObj -= dt;
    }
  }

  // ||p||^2 = R^2 => ||o + td||^2 - R^2 = 0 => t^2||d||^2 + 2t(d.o) + (||o||^2 - R^2) = 0 => at^2 + bt + c = 0
  // Then, a = ||d||^2, b = 2(d.o), c = ||o||^2 - R^2, where ||n||^2 = dot(n, n)
  const Intervalf a{ computeDot(rayDirVec3fi, rayDirVec3fi) };
  const Intervalf b{ Intervalf{ 2 } * computeDot(rayDirVec3fi, rayOriginPoint3fi) };
  const Intervalf c{ computeDot(rayOriginPoint3fi, rayOriginPoint3fi) - (Intervalf{ m_radius } * Intervalf{ m_radius }) };

  // Since we're in object space, we use an alternate stable form of the quadratic equation (QE), i.e. the closest-approach method (a reparameterization to avoid catastrophic cancellation in evaluating the discriminant: b^2 - 4ac, by using f = b/(2a))
  const Intervalf f{ b / (Intervalf{ 2 } * a) };
  // Get the vector interval from the sphere center to the closest point on the ray and get its length (a vector interval)
  const Vec3fi v{ rayOriginPoint3fi[0] - (f * rayDirVec3fi[0]), rayOriginPoint3fi[1] - (f * rayDirVec3fi[1]), rayOriginPoint3fi[2] - (f * rayDirVec3fi[2]) };
  const Intervalf len{ sqrtI(computeDot(v, v)) };

  // Compute the QE's discr: 4a * (R + |v|) * (R - |v|)
  const Intervalf discr{ Intervalf{ 4 } * a * (Intervalf{ m_radius } + len) * (Intervalf{ m_radius } - len) };
 
  // If the lower bound of the discr. is negative, then the ray misses the sphere entirely as the smallest possible value fails to yield a real root
  if (discr.getLower() < Float{}) return std::nullopt;

  // Compute the two roots of the QE using the stable form where q = -0.5(b +/- sqrt(discr)) and using Viete's relations to get t0 = q/a, t1 = c/q. b's sign decides which root to use for q
  const Intervalf minusHalf{ Float{ -0.5 } };
  Intervalf q{ (b.getMid() < Float{}) ? minusHalf * (b - sqrtI(discr)) : minusHalf * (b + sqrtI(discr)) };
  
  Intervalf firstCandidateRoot{ q / a }, secondCandidateRoot{ c / q };

  // If any root is closer to the origin, check that first by setting it as t0. Return early if both candidates are OOB
  if (firstCandidateRoot.getLower() > secondCandidateRoot.getLower()) std::swap(firstCandidateRoot, secondCandidateRoot);

  if (firstCandidateRoot.getUpper() > rayParamTMaxObj || secondCandidateRoot.getLower() <= Float{}) return std::nullopt;

  // Set a root, evaluate the angles and check for clipping
  Intervalf finalCandidateRoot{ firstCandidateRoot };
  bool usedFront{ true };

  if (finalCandidateRoot.getLower() <= Float{})
  { 
    finalCandidateRoot = secondCandidateRoot;
    usedFront = false; 
    if (finalCandidateRoot.getUpper() > rayParamTMaxObj) return std::nullopt;
  }

  // Convert interval ray to float using midpoints for intersection point computation
  const Point3f rayOriginP3f{ rayOriginPoint3fi };
  const Vec3f rayDirV3f{ rayDirVec3fi };

  auto computePHitPhi = [&](const Intervalf& tI, Point3f& pOut, Float& phiOut) 
  {
    const Float tHit{ tI.getMid() };
    pOut = rayOriginP3f + (rayDirV3f * tHit);
    pOut *= m_radius * (Float{ 1 } / euclideanLength(pOut));

    if (pOut[0] == Float{} && pOut[1] == Float{}) pOut[0] = kSafeNormalizeLen<Float> * m_radius;

    phiOut = std::atan2(pOut[1], pOut[0]); 
    
    if (phiOut < Float{}) phiOut += Float{ 2 } * kPi;
  };

  auto clipped = [&](const Point3f& pLoc, Float phiLoc) 
  {
    const bool zBad{ ((m_zMin > -m_radius) && (pLoc[2] < m_zMin)) || ((m_zMax < m_radius) && (pLoc[2] > m_zMax)) };
    
    return zBad || (phiLoc > m_phiMax);
  };

  Point3f p{}; Float phi{}; 
  
  computePHitPhi(finalCandidateRoot, p, phi);
  
  if (clipped(p, phi)) 
  {
    if (!usedFront) return std::nullopt;
    if (secondCandidateRoot.getUpper() > rayParamTMaxObj) return std::nullopt;
    
    finalCandidateRoot = secondCandidateRoot;
    
    computePHitPhi(finalCandidateRoot, p, phi);
    
    if (clipped(p, phi)) return std::nullopt;
  }

  return QuadricIntersection{ finalCandidateRoot.getMid(), p, phi };
}

SurfaceInteraction Sphere::interactionFromIntersection(const QuadricIntersection& quadricIntersection, [[maybe_unused]] const Vec3f& w_oWorld, Float timeSec) const
{
  const Point3f pObj{ quadricIntersection.pHitPos };
  const Point3f pRen{ m_renderFromObject(pObj) };

  const Float phi{ quadricIntersection.phi };
  const Float u{ (m_phiMax > 0) ? (phi / m_phiMax) : Float{} };

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

std::optional<ShapeIntersection> Sphere::intersect(const Ray& ray, Float tMax) const 
{
  const auto quadricIntersection{ basicIntersect(ray, tMax) };
  
  if (!quadricIntersection) return std::nullopt;

  const auto surfaceInteraction{ interactionFromIntersection(*quadricIntersection, -ray.getDirection(), ray.getTimeSeconds()) };

  return ShapeIntersection{ std::move(surfaceInteraction), quadricIntersection->tHit };
}
