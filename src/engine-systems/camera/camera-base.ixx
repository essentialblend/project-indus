export module camerabase;

import ray;
import transform;
import cameratransform;
import cameraconstructs;
import samplingconstructs;
import filmbase;
import types;

// Omitted/TODO: Animated transforms (CameraTransform), Mediums, RDs

export class CameraBase 
{
public:
  virtual ~CameraBase() = default;

  // Generate a primary ray for a sample. Returns weight.
  virtual CameraRay generateRay(const CameraSample&) const = 0;

  [[nodiscard]] const Transform4f& applyCameraToWorld() const noexcept;
  [[nodiscard]] const CameraTransform& getCameraTransform() const noexcept;
  [[nodiscard]] CameraShutter getShutter() const noexcept;
  [[nodiscard]] FilmBase& getFilm() const noexcept;

protected:
  CameraBase(const CameraTransform&, const Transform4f&, const CameraShutter&, FilmBase&) noexcept;

  Transform4f m_cameraToWorld{};
  CameraTransform m_cameraTransform{};
  CameraShutter m_cameraShutter{};
  FilmBase& m_film;
};

CameraBase::CameraBase(const CameraTransform& camTransform, const Transform4f& camToWorld, const CameraShutter& shutter, FilmBase& film) noexcept : m_cameraTransform{ camTransform }, m_cameraToWorld{ camToWorld }, m_cameraShutter{ shutter }, m_film{ film } {}

const Transform4f& CameraBase::applyCameraToWorld() const noexcept
{
  return m_cameraToWorld;
}

const CameraTransform& CameraBase::getCameraTransform() const noexcept
{
  return m_cameraTransform;
}

CameraShutter CameraBase::getShutter() const noexcept
{
  return m_cameraShutter;
}

FilmBase& CameraBase::getFilm() const noexcept
{
  return m_film;
}