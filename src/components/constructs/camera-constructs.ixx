export module cameraconstructs;

import std;
import types;
import ray;

export
{
  enum class ColorEncoding { Linear, sRGB, Gamma22 };

  struct CameraShutter final
  {
    Float shutterOpen{};
    Float shutterClose{};
  };

  enum class RenderingSpace { World, CameraWorld, Camera };

  struct FilmConfig final
  {
    Point2i resolution{};
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
  };
}