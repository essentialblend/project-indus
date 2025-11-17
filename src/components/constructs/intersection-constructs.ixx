export module intersectionconstructs;

import types;
import std;
import point;
import surfaceinteraction;

export
{
  struct QuadricIntersection final
  {
    Float tHit{};
    Point3f pHitPos{};
    Float phi{};
  };

  struct RayBoxHit
  {
    Float tEnter{};
    Float tExit{};
  };

  struct SphereBounds
  {
    Point3f sphereCenter;
    Float sphereRadius{};
  };

  struct ShapeIntersection final
  {
    SurfaceInteraction interaction;
    Float tHit{};
  };

  enum class BVHSplitMethod : std::uint8_t
  {
    SAH,
    Middle,
    EqualCounts
  };
}