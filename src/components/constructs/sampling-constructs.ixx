export module samplingconstructs;

import std;
import types;
import colorrgb;

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

  struct MMIXLCG final
  {
    static constexpr unsigned modulusBits{ 64 };
    static constexpr std::uint64_t multiplier{ 0x5851'F42D'4C95'7F2DULL };
    static constexpr std::uint64_t increment{ 0x1405'7B7E'F767'814FULL };
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