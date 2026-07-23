export module indus.geom.shape;

import std;

import indus.core.types;
import indus.core.geom.transform;
import indus.core.geom.bounds;
import indus.core.geom.ray;

import indus.geom.surfaceinteraction;
import indus.geom.intersection_constructs;

export class Shape
{
public:
  virtual ~Shape() = default;

  [[nodiscard]] virtual std::optional<ShapeIntersection> intersect(const Ray& ray) const = 0;

  [[nodiscard]] virtual bool intersectP(const Ray& ray) const = 0;
  
  [[nodiscard]] virtual SurfaceInteraction interactionFromIntersection(const QuadricIntersection&, const Vec3f& w_oWorld, Float timeSec) const = 0;
  [[nodiscard]] virtual Bounds3f getBounds() const noexcept = 0;
  
  // TBD: Float Area() const; DirectionCone NormalBounds() const; sampling methods
};
