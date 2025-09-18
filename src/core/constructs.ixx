export module constructs;

import std;

import types;
import vector;
import ray;
import transform;

// Structs
export struct CameraSample
{
  Point2f pFilm;
  Point2f pLens;
  Float time;
};

export struct FilmConfig
{
  Point2i resolution{};
  std::string filename{};

  constexpr Float aspect() const noexcept 
  { 
    return static_cast<Float>(resolution[0]) / std::max(1, resolution[1]);
  }
};

export struct CameraShutter
{
  Float shutterOpen{};
  Float shutterClose{};
};

export struct CameraConfig
{
  Transform4f cameraToWorld;
  Float fovDegrees{};
  CameraShutter cameraShutter{};
  Float lensRadius{};
  Float focalDistance{};
  Bounds2f screenWindow{};
};

export struct SamplerConfig
{
  Int samplesPerPixel{};
  bool stratified{ false };
};

export struct IntegratorConfig
{
  Idx maxDepth{};
  bool useRR{ false };
};

export struct IndusConfig
{
  FilmConfig filmCfg{};
  CameraConfig camCfg{};
  SamplerConfig samplerCfg{};
  IntegratorConfig integratorCfg{};
};

export enum class ColorEncoding { Linear, sRGB, Gamma22 };

export struct CameraRay
{
  Ray ray{};
  Float weight{};
};

export struct MMIXLCG
{
  static constexpr unsigned modulusBits{ 64 };
  static constexpr std::uint64_t multiplier{ 0x5851'F42D'4C95'7F2DULL };
  static constexpr std::uint64_t increment{ 0x1405'7B7E'F767'814FULL };
};

export struct Strata2D
{
  Int NX{};
  Int NY{};

  [[nodiscard]] constexpr Int getTotal() const noexcept
  {
    return NX * NY;
  }
};