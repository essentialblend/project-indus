export module camerabase;

import ray;
import transform;
import constructs;
import film;
import types;

// Omitted/TODO: Animated transforms (CameraTransform), Mediums, RDs

export class CameraBase 
{
public:
  virtual ~CameraBase() = default;

  // Generate a primary ray for a sample. Returns weight.
  virtual CameraRay generateRay(const CameraSample&) const = 0;

  [[nodiscard]] const Transform4f& cameraToWorld() const noexcept;
  [[nodiscard]] CameraShutter getShutter() const noexcept;
  [[nodiscard]] Film& film() const noexcept;

protected:
  CameraBase(const Transform4f&, const CameraShutter&, Film&) noexcept;

  Transform4f m_cameraToWorld;
  CameraShutter m_cameraShutter;
  Film& m_film;
};

CameraBase::CameraBase(const Transform4f& camToWorld, const CameraShutter& shutter, Film& film) noexcept : m_cameraToWorld{ camToWorld }, m_cameraShutter{ shutter }, m_film{ film } {}

const Transform4f& CameraBase::cameraToWorld() const noexcept
{
  return m_cameraToWorld;
}

CameraShutter CameraBase::getShutter() const noexcept
{
  return m_cameraShutter;
}

Film& CameraBase::film() const noexcept
{
  return m_film;
}