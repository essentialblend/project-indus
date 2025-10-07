export module shape;

import std;
import surfaceinteraction;
import types;
import transform;

export struct ShapeIntersection final 
{
  SurfaceInteraction interaction;
  Float tHit{};
};

export class Shape
{
public:
  virtual ~Shape() = default;

  [[nodiscard]] virtual std::optional<ShapeIntersection> intersect(const Ray& ray, Float rayParamTMax) const = 0;

  [[nodiscard]] virtual bool intersectP(const Ray& ray, Float rayParamTMax) const = 0;

  //[[nodiscard]] virtual Bounds3f getBounds() const noexcept = 0;
};
