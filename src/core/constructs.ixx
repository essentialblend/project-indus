export module constructs;

import std;

import types;
import vector;
import ray;
import transform;
import colorrgb;


// Structs
export struct CameraSample final
{
  Point2f pFilm;
  Point2f pLens;
  Float time;
};

export struct FilmConfig final
{
  Point2i resolution{};
  std::string filename{};

  constexpr Float aspect() const noexcept 
  { 
    return static_cast<Float>(resolution[0]) / std::max(1, resolution[1]);
  }
};

export struct CameraShutter final
{
  Float shutterOpen{};
  Float shutterClose{};
};

export struct CameraConfig final
{
  Transform4f cameraToWorld;
  Float fovDegrees{};
  CameraShutter cameraShutter{};
  Float lensRadius{};
  Float focalDistance{};
  Bounds2f screenWindow{};
};

export struct Strata2D final
{
  Int NX{};
  Int NY{};

  [[nodiscard]] constexpr Int64 getTotal() const noexcept
  {
    return NX * NY;
  }
};

export struct SamplerConfig final
{
  Int samplesPerPixel{};
  bool isStratified{ true };
  Strata2D strata{ 1, 1 };
  bool isJitter{ true };
};

export struct IntegratorConfig final
{
  Idx maxDepth{};
  bool useRR{ false };
};

export struct IndusConfig final
{
  FilmConfig filmCfg{};
  CameraConfig camCfg{};
  SamplerConfig samplerCfg{};
  IntegratorConfig integratorCfg{};
};

export enum class ColorEncoding { Linear, sRGB, Gamma22 };

export struct CameraRay final
{
  Ray ray{};
  Float weight{};
};

export struct MMIXLCG final
{
  static constexpr unsigned modulusBits{ 64 };
  static constexpr std::uint64_t multiplier{ 0x5851'F42D'4C95'7F2DULL };
  static constexpr std::uint64_t increment{ 0x1405'7B7E'F767'814FULL };
};

export enum class BxDFType : std::uint8_t
{
  Reflection = 1 << 0,
  Transmission = 1 << 1,
  Diffuse = 1 << 2,
  Glossy = 1 << 3,
  Specular = 1 << 4
};

export struct BSDFSample final
{
  std::optional<Vec3f> unitW_iLocal{};
  std::optional<Vec3f> unitW_iWorld{};

  ColorRGB BRDF{};
  Float PDF{};
  BxDFType flags{};
};

export struct RefractResult
{
  Vec3f unitW_iLocal{};
  Float etaPrime{};
};