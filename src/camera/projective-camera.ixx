export module projectivecamera;

import types;

import std;
import camerabase;
import film;
import constructs;
import mathalgebra;
import mathtrig;

// Omitted (TODOs):
// - RayDifferential generation.
// - Importance sampling methods (We, PDF_We, SampleWi).

export class ProjectiveCamera : public CameraBase 
{
public:
  virtual ~ProjectiveCamera() = default;

protected:
  ProjectiveCamera(const CameraTransform&, const Transform4f&, const CameraShutter&, Film&, const Transform4f&, const Bounds2f&, Float, Float) noexcept;

  // Derived classes must implement ray generation.
  CameraRay generateRay(const CameraSample&) const override = 0;
  
  // Projection transforms
  Transform4f m_screenFromCamera;
  Transform4f m_rasterFromScreen;
  Transform4f m_screenFromRaster;
  
  Transform4f m_cameraFromRaster;
  
  Float m_lensRadius;
  Float m_focalDistance;
};

ProjectiveCamera::ProjectiveCamera(const CameraTransform& cameraTransform, const Transform4f& camToWorld, const CameraShutter& shutter, Film& film, const Transform4f& screenFromCamera, const Bounds2f& screenWindow, Float lensRadius, Float focalDistance) noexcept : CameraBase(cameraTransform, camToWorld, shutter, film), m_screenFromCamera{ screenFromCamera }, m_lensRadius{ lensRadius }, m_focalDistance{ focalDistance }
{
  const Float xmin{ std::min(screenWindow[0][0], screenWindow[1][0]) };
  const Float xmax{ std::max(screenWindow[0][0], screenWindow[1][0]) };
  const Float ymin{ std::min(screenWindow[0][1], screenWindow[1][1]) };
  const Float ymax{ std::max(screenWindow[0][1], screenWindow[1][1]) };

  // Screen to NDC
  Transform4f NDCFromScreen
  {
    Transform4f::scale({ Float(1) / (xmax - xmin), Float(1) / (ymax - ymin), Float(1) }) * Transform4f::translate({ -xmin, -ymax, Float{} })
  };

  // NDC to raster
  Transform4f rasterFromNDC
  {
    Transform4f::scale({ Float(film.getFilmResolution()[0]), -Float(film.getFilmResolution()[1]), Float(1) })
  };

  m_rasterFromScreen = rasterFromNDC * NDCFromScreen;
  m_screenFromRaster = Transform4f{ m_rasterFromScreen.getInv(), m_rasterFromScreen.get() };

  m_cameraFromRaster = Transform4f{ m_screenFromCamera.getInv(), m_screenFromCamera.get() } * m_screenFromRaster;
}