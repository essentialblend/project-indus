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
  enum class FilterPreset { Light, Balanced, Heavy };

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
    FilterType filterType{ FilterType::Gaussian };
    FilterPreset filterPreset{ FilterPreset::Balanced };
    Point2i resolution{};
    Bounds2i crop{};
    float diagonalMM{};
    Float imagingRatio{ Float{ 1 } };
    std::string filename{};

    constexpr Float aspect() const noexcept
    {
      return static_cast<Float>(resolution[0]) / std::max(1, resolution[1]);
    }
  };

  struct FilterConfig
  {
    FilterConfig(const FilterType& filterType, const FilterPreset& filterPreset)
    {
      switch (filterType)
      {
      case FilterType::Box:
        switch(filterPreset)
        {
        case FilterPreset::Light:
          supportRadius = Vec2f{ 0.75 };
          break;
        case FilterPreset::Balanced:
          supportRadius = Vec2f{ 1.5 };
          break;
        case FilterPreset::Heavy:
          supportRadius = Vec2f{ 2.5 };
          break;
        }
        break;
      case FilterType::Gaussian:
        switch (filterPreset)
        {
        case FilterPreset::Light:
          supportRadius = Vec2f{ 0.75 };
          gaussianKernelSigma = Float{ 0.35 };
          break;
        case FilterPreset::Balanced:
          supportRadius = Vec2f{ 1.5 };
          gaussianKernelSigma = Float{ 0.5 };

          break;
        case FilterPreset::Heavy:
          supportRadius = Vec2f{ 2.5 };
          gaussianKernelSigma = Float{ 0.9 };
          break;
        }
        break;
      }
    }

    Vec2f supportRadius{};
    Float gaussianKernelSigma{};
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