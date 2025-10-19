export module shape;

import std;
import surfaceinteraction;
import types;
import transform;
import intersectionconstructs;
import bounds;
import ray;

export class Shape
{
public:
  virtual ~Shape() = default;

  [[nodiscard]] virtual std::optional<ShapeIntersection> intersect(const Ray& ray) const = 0;

  [[nodiscard]] virtual bool intersectP(const Ray& ray) const = 0;
  
  [[nodiscard]] virtual SurfaceInteraction interactionFromIntersection(const QuadricIntersection&, const Vec3f& w_oWorld, Float timeSec) const = 0;
  [[nodiscard]] virtual Bounds3f getBounds() const noexcept = 0;
  
  // later: Float Area() const; DirectionCone NormalBounds() const; sampling methods
};
