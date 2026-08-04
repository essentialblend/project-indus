export module indus.engine.constructs;

import std;

import indus.core.types;
import indus.core.geom.bounds;
import indus.core.geom.transform;

import indus.camera.constructs;

import indus.sampling.constructs;

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

  enum class SinkType
  {
    SFML
  };

  enum class AggregateType
  {
    SAHBVH,
    List
  };

  struct IntegratorConfig final
  {
    Idx maxDepth{};
    bool useRR{ true };
  };

  struct DisplaySinkConfig final
  {
    SinkType sinkType{};
    Point2i windowResolution{};
  };

  struct IndusConfig final
  {
    FilmConfig filmCfg{};
    CameraConfig camCfg{};
    SamplerConfig samplerCfg{};
    IntegratorConfig integratorCfg{};
    DisplaySinkConfig displaySinkCfg{};
    std::size_t runtimeThreads{};
    AggregateType aggregateType{ AggregateType::SAHBVH };
    bool headless{ false };
    bool writeImage{ true };
  };

  struct PhysicalUnits final
  {
    // 1 Engine Unit = 10 millimeters = 1 centimeter
    Float unitLengthInMM{ 10 };
  };

  enum class HUDMode{ Hidden, Strip, Detailed };

  struct EngineBuildInformation
  {
    std::string_view engineName;
    std::string_view engineVersion;
    std::string_view buildConfig;
    std::string_view gitHash;
    std::string_view buildTimestamp;
    std::string_view author;
    UInt32 runtimeThreads{ 0 };
  };

  struct RenderSystemStrings
  {
    std::string scheduler{};
    std::string sampler{};
    std::string rng{};
    std::string transport{};
    std::string accelerator{};
    std::string camera{};
    std::string film{};
    std::string filter{};
    std::string display{};
  };

}
