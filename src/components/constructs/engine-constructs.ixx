export module engineconstructs;

import std;
import types;
import cameraconstructs;
import samplingconstructs;
import bounds;
import transform;
import threadpool;
import image;

export
{
  struct CameraConfig final
  {
    Transform4f cameraToWorld;
    Float fovDegrees{};
    CameraShutter cameraShutter{};
    Float lensRadius{};
    Float focalDistance{};
    Bounds2f screenWindow{};
    RenderingSpace renderingSpace{ RenderingSpace::World };
  };

  struct SamplerConfig final
  {
    Int samplesPerPixel{};
    bool isStratified{ true };
    Strata2D strata{ 1 };
    bool isJitter{ true };
  };

  struct IntegratorConfig final
  {
    Idx maxDepth{};
    bool useRR{ true };
  };

  struct IndusConfig final
  {
    FilmConfig filmCfg{};
    CameraConfig camCfg{};
    SamplerConfig samplerCfg{};
    IntegratorConfig integratorCfg{};
  };

  struct PhysicalUnits final
  {
    // 1 Engine Unit = 10 millimeters = 1 centimeter
    Float unitLengthInMM{ 10 };
  };

  struct FrameSnapshot final
  {
    Image image{};
    float progressUnitNormalized{};
    std::uint64_t frameVersion{};
  };

  using DisplayConsumer = std::function<void(FrameSnapshot)>;
 
}
