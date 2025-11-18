export module indus.sampling.constructs;

import std;

import indus.core.types;
import indus.core.colorrgb;

export
{
  struct CameraSample final
  {
    Point2f pFilm;
    Point2f pLens;
    Float time;
  };

  struct Strata2D final
  {
    Int NX{};
    Int NY{};

    constexpr explicit Strata2D(Int x) : NX{ x }, NY{ x } {}

    [[nodiscard]] constexpr Int getTotal() const noexcept
    {
      return Int{ NX * NY };
    }
  };

  enum class BxDFType : std::uint8_t
  {
    Reflection = 1 << 0,
    Transmission = 1 << 1,
    Diffuse = 1 << 2,
    Glossy = 1 << 3,
    Specular = 1 << 4
  };

  struct BSDFSample final
  {
    std::optional<Vec3f> unitW_iLocal{};
    std::optional<Vec3f> unitW_iWorld{};

    ColorRGB BRDF{};
    Float PDF{};
    BxDFType flags{};
  };

  struct RefractResult final
  {
    Vec3f unitW_iLocal{};
    Float etaPrime{};
  };
}