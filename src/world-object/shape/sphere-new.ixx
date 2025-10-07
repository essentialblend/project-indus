export class spherenew;

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

  [[nodiscard]] SurfaceInteraction interactionFromIntersection(const QuadricIntersection& quadricIntersection, const Vec3f& w_oWorld, Float timeSec) const;
};

Sphere::Sphere(const Transform4f& renderFromObject, const Transform4f& objectFromRender, bool reverse, Float radius, Float zMin, Float zMax, Float phiMax) noexcept : m_renderFromObject{ renderFromObject }, m_objectFromRender{ objectFromRender }, m_reverseOrientation{ reverse }, m_swapsHandedness{ renderFromObject.swapsHandedness() }, m_radius{ radius }, m_zMin{ std::min(zMin, zMax) }, m_zMax{ std::max(zMin, zMax) }, m_phiMax{ degreesToRadians(clamp(phiMax, Float{ 0 }, Float{ 360 })) } {}

//Bounds3f Sphere::getBounds() const noexcept 
//{
//  const Point3f pMin{ -m_radius, -m_radius, m_zMin };
//  const Point3f pMax{ m_radius,  m_radius, m_zMax };
//  
//  return m_renderFromObject(Bounds3f{ pMin, pMax });
//}

bool Sphere::intersectP(const Ray& r, Float tMax) const 
{
  return basicIntersect(r, tMax).has_value();
}

std::optional<QuadricIntersection> Sphere::basicIntersect(const Ray& ray, Float rayParamTMax) const
{
  // oc = o - c left out as we're in object space 
  const Point3fi rayOriginInterval{ m_objectFromRender(Point3fi{ std::array<Intervalf,3>{ makeInterval(ray.getOrigin()[0]), makeInterval(ray.getOrigin()[1]), makeInterval(ray.getOrigin()[2]) } }) };

  const Vec3fi rayDirInterval{ m_objectFromRender(Vec3fi{ std::array<Intervalf,3>{ makeInterval(ray.getDirection()[0]), makeInterval(ray.getDirection()[1]), makeInterval(ray.getDirection()[2]) } }) };
  
  const Intervalf R{ m_radius };

  const Intervalf a{ computeDot(rayDirInterval, rayDirInterval) };
  const Intervalf b{ Intervalf{ Float{ 2 } } * computeDot(rayDirInterval, rayOriginInterval) };
  const Intervalf c{ computeDot(rayOriginInterval, rayOriginInterval) - (R * R) };

  const Intervalf f{ b / (Intervalf{ Float{ 2 } } * a) };
  const Vec3fi v{ std::array<Intervalf,3>{ rayOriginInterval[0] - (f * rayDirInterval[0]), rayOriginInterval[1] - (f * rayDirInterval[1]), rayOriginInterval[2] - (f * rayDirInterval[2]) } };
  const Intervalf len{ sqrtI(computeDot(v, v)) };
  const Intervalf disc{ Intervalf{ Float{ 4 } } * a * (R + len) * (R - len) };
  
  if (disc.getLower() < Float{}) return std::nullopt;

  const Intervalf root{ sqrtI(disc) };
  const Intervalf q{ (b.getMid() < Float{}) ? Intervalf{ Float{ -0.5 } } * (b - root) : Intervalf{ Float{ -0.5 } } * (b + root) };

  Intervalf t0{ q / a }; Intervalf t1{ c / q };

  if (t0.getLower() > t1.getLower()) std::swap(t0, t1);

  auto tryRoot = [&](const Intervalf& tI) -> std::optional<QuadricIntersection> 
  {
    if (tI.getUpper() > rayParamTMax || tI.getLower() <= Float{}) return std::nullopt;
    const Float tHit{ Float{ 0.5 } * (tI.getLower() + tI.getUpper()) };

    Point3f p{ Point3f{ ray.getOrigin() + (ray.getDirection() * tHit) } };

    p *= (m_radius * (Float{ 1 } / euclideanLength(p)));

    if (p[0] == Float{} && p[1] == Float{}) p[0] = Float{ 1e-5f } * m_radius;

    Float phi{ std::atan2(p[1], p[0]) }; 
    
    if (phi < Float{}) phi += Float{ 2 } * kPi;

    const bool outsideZ{ ((m_zMin > -m_radius) && (p[2] < m_zMin)) || ((m_zMax < m_radius) && (p[2] > m_zMax)) };

    if (outsideZ || phi > m_phiMax) return std::nullopt;

    return QuadricIntersection{ tHit, p, phi };
  };

  if (const auto h0{ tryRoot(t0) }) return h0;
  if (const auto h1{ tryRoot(t1) }) return h1;

  return std::nullopt;
}

SurfaceInteraction Sphere::interactionFromIntersection(const QuadricIntersection& quadricIntersection, [[maybe_unused]] const Vec3f& w_oWorld, Float timeSec) const
{
  const Float rayParamTHit{ quadricIntersection.rayParamT };
  Point3f p{ quadricIntersection.pHitPos };
  const Float phi{ quadricIntersection.phi };
  
  const Float u{ (m_phiMax > 0) ? (phi / m_phiMax) : Float{} };
  
  const Float theta{ safeACos(p[2] / m_radius) };
  const Float thetaMin{ safeACos(m_zMax / m_radius) };
  const Float thetaMax{ safeACos(m_zMin / m_radius) };
  
  const Float v{ (thetaMax != thetaMin) ? ((theta - thetaMin) / (thetaMax - thetaMin)) : Float{} };

  Normal3f n{ normalize(Normal3f{ p[0] / m_radius, p[1] / m_radius, p[2] / m_radius}) };
  
  if (m_reverseOrientation ^ m_swapsHandedness) 
    n = Normal3f{ -n[0], -n[1], -n[2] };

  const Vec3f pErr{ gamma<Float>(5) * std::abs(p[0]), gamma<Float>(5) * std::abs(p[1]), gamma<Float>(5) * std::abs(p[2]) };

  // Constructor sets shading basis, so setting it isn't required
  SurfaceInteraction surfaceInteraction{ p, timeSec, rayParamTHit, pErr, n };
  surfaceInteraction.setUV(Point2f{ u, v });

  return surfaceInteraction;
}

std::optional<ShapeIntersection> Sphere::intersect(const Ray& ray, Float tMax) const 
{
  auto qi{ basicIntersect(ray, tMax) };
  
  if (!qi) return std::nullopt;

  SurfaceInteraction surfaceInteraction{ interactionFromIntersection(*qi, -ray.getDirection(), ray.getTimeSeconds()) };

  return ShapeIntersection{ std::move(surfaceInteraction), qi->rayParamT };
}
