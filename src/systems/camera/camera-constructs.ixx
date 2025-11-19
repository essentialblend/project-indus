export module indus.camera.constructs;

import std;

import indus.core.types;
import indus.core.geom.ray;
import indus.core.geom.bounds;
import indus.core.geom.squarematrix;
import indus.core.colorrgb;

export
{
  enum class FilterType { Box, Gaussian };

  struct PhysicalUnits final
  {
    // 1 Engine Unit = 10 millimeters = 1 centimeter
    Float unitLengthInMM{ 10 };
  };

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
    FilterType filterType{ FilterType::Gaussian };

    constexpr Float aspect() const noexcept
    {
      return static_cast<Float>(resolution[0]) / std::max(1, resolution[1]);
    }
  };

  struct CameraRay final
  {
    Ray ray{};
    Float weight{};

    // Needs a principled solution, a slight wrangle to pick a low hanging fruit from Realistic Camera
    Float exposureScale{};
  };

  struct FilterSample
  {
    Point2f pOffset{};
    Float weightOverPDF{};
  };

  enum class PixelFormat { U8, F16, F32 };

  struct ImageMetadata 
  {
    std::optional<float> renderTimeSeconds{};
    std::optional<int> samplesPerPixel{};
    std::optional<Point2i> fullResolution{};
    std::optional<Bounds2i> pixelBounds{};
  };
}