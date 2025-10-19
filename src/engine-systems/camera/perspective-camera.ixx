export module perspectivecamera;

import samplingutil;

import projectivecamera;
import types;
import vector;
import mathalgebra;
import mathfp;


// TODO: ray differentials, spectral transport

export class PerspectiveCamera : public ProjectiveCamera
{
public:
  PerspectiveCamera(const CameraTransform&, const Transform4f&, const CameraShutter&, Film&, Float, const Bounds2f&, Float, Float) noexcept;

  CameraRay generateRay(const CameraSample&) const override;

private:
  Vec3f m_dxCamera{};
  Vec3f m_dyCamera{};
  
  Float m_cosTotalWidth{};
  Float m_imagePlaneArea{};
};

PerspectiveCamera::PerspectiveCamera(const CameraTransform& cameraTransform, const Transform4f& camToWorld, const CameraShutter& shutter, Film& film, Float fovDegrees, const Bounds2f& screenWindow, Float lensRadius, Float focalDistance) noexcept : ProjectiveCamera(cameraTransform, camToWorld, shutter, film, Transform4f::perspective(fovDegrees, static_cast<Float>(1e-2), static_cast <Float>(1000.0)), screenWindow, lensRadius, focalDistance)
{

  Point3f origin{};

  m_dxCamera = m_cameraFromRaster(Point3f{ 1, 0, 0 }) - m_cameraFromRaster(origin);
  m_dyCamera = m_cameraFromRaster(Point3f{ 0, 1, 0 }) - m_cameraFromRaster(origin);

  //// Corner direction
  //Point2f filterRadius{ film.getFilmResolution()[0] * 0.5, film.getFilmResolution()[1] * 0.5 };
  //Point3f pCorner{ -filterRadius[0], -filterRadius[1], 0.0 };
  //Point3f pCamera = m_cameraFromRaster(pCorner);
  //Vec3f wCorner{ normalize(Vec3f{ pCamera[0], pCamera[1], pCamera[2] }) };
  //m_cosTotalWidth = wCorner[2];

  //// Image plane area at z=1
  //Point3f pMin{ m_cameraFromRaster(Point3f{ 0,0,0 }) };
  //Point3f pMax{ m_cameraFromRaster(Point3f{ film.getFilmResolution()[0], film.getFilmResolution()[1], 0 }) };
  //pMin = Point3f{ {pMin[0] * (Float(1.0) / pMin[2]), pMin[1] * (Float(1.0) / pMin[2])} };
  //pMax = Point3f{ {pMin[0] * (Float(1.0) / pMax[2]), pMin[1] * (Float(1.0) / pMax[2])} };
  //m_imagePlaneArea = std::abs((pMax[0] - pMin[0]) * (pMax[1] - pMin[1]));
}

CameraRay PerspectiveCamera::generateRay(const CameraSample& cs) const
{
  // Point3f lensRayOrigin{};
  
  // Raster to camera
  const Point3f pRaster{ cs.pFilm[0], cs.pFilm[1], Float{} };
  const Point3f pCamera{ m_cameraFromRaster(pRaster) };

  Point3f originCam{};
  Vec3f unitDirCam{ normalize(Vec3f{ pCamera[0], pCamera[1], pCamera[2] }) };

  // Defocus blur (lensRadius = 0 and defocus blur is disabled)
  if (m_lensRadius > Float{})
  {
    const auto& lensSample{ concentricSampleDisk(cs.pLens) };
    const Point2f pLens{ lensSample[0] * m_lensRadius, lensSample[1] * m_lensRadius };
    originCam = Point3f{ pLens[0], pLens[1], Float{} };

    const Float ft{ m_focalDistance / unitDirCam[2] };
    const Point3f pFocus{ originCam + (unitDirCam * ft) };
    unitDirCam = normalize(pFocus - originCam);
  }

  const Float t{ lerp(cs.time, m_cameraShutter.shutterOpen, m_cameraShutter.shutterClose) };

  const Ray cameraSpaceRay{ originCam, unitDirCam, t };
  const Ray renderSpaceRay{ m_cameraTransform.applyRenderFromCamera(cameraSpaceRay) };

  return CameraRay{ renderSpaceRay, Float{ 1.0 } };
}