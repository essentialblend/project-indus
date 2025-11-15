export module engineconstructs;

import std;
import types;
import cameraconstructs;
import samplingconstructs;
import bounds;
import transform;
import threadpool;
import image;
import statconstructs;

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
    std::optional<RenderStats> renderStats{};
  };

  using DisplayConsumer = std::function<void(FrameSnapshot)>;
 
  enum class HUDMode{ Hidden, Strip, Detailed };

  struct EngineBuildInformation
  {
    std::string_view engineName;
    std::string_view engineVersion;
    std::string_view buildConfig;
    std::string_view gitHash;
    std::string_view buildTimestamp;
    std::string_view author;
    std::uint32_t runtimeThreads{ 0 };
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
