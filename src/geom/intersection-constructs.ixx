export module indus.geom.intersection_constructs;

import std;

import indus.core.types;
import indus.core.geom.point;

import indus.geom.surface_interaction;

export
{
  struct QuadricIntersection final
  {
    Float tHit{};
    Point3f pHitPos{};
    Float phi{};
  };

  struct ShapeIntersection final
  {
    SurfaceInteraction interaction;
    Float tHit{};
  };

  enum class BVHSplitMethod : UInt8
  {
    SAH,
    Middle,
    EqualCounts
  };
}