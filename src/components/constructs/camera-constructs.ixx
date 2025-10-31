export module cameraconstructs;

import std;
import types;
import ray;
import bounds;
import squarematrix;

export
{
  enum class ColorEncoding { Linear, sRGB };

  struct CameraShutter final
  {
    Float shutterOpen{};
    Float shutterClose{};
  };

  enum class RenderingSpace { World, CameraWorld, Camera };

  struct FilmConfig final
  {
    Point2i resolution{};
    Bounds2i crop{};
    float diagonalMM{};
    Vec2f filterRadius{ Float{ 0.5 }, Float{ 0.5 } };
    Float imagingRatio{ Float{ 1 } };
    std::string filename{};

    constexpr Float aspect() const noexcept
    {
      return static_cast<Float>(resolution[0]) / std::max(1, resolution[1]);
    }
  };

  struct CameraRay final
  {
    Ray ray{};
    Float weight{};
    // Needs a principled solution. Currently, a slight wrangle to pick a low hanging fruit from Realistic Camera
    Float exposureScale{};
  };

  struct FilterSample
  {
    Point2f pOffset{};
    Float weightOverPDF{};
  };

  struct RGBCIE1931Vertices
  {
    Vec2f xRyR{};
    Vec2f xGyG{};
    Vec2f xByB{};
    Vec2f xWyW{};
  };

  struct RGBColorSpace
  {
    RGBCIE1931Vertices RGBVertices{};

    Mat3d XYZFromRGB{};
    Mat3d RGBFromXYZ{};

    Vec3d luminanceFromRGB{};
  };
}