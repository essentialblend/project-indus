export module perspectivecamera;

import samplingutil;

import projectivecamera;
import types;
import vector;
import mathalgebra;
import mathfp;
import filmbase;
import mathtrig;
import mathconstants;
import engineconstructs;

// TODO: ray differentials, spectral transport

export class PerspectiveCamera : public ProjectiveCamera
{
public:
  PerspectiveCamera(const CameraTransform&, const Transform4f&, const CameraShutter&, FilmBase&, Float, const Bounds2f&, Float, Float) noexcept;

  CameraRay generateRay(const CameraSample&) const override;

  [[nodiscard]] std::string toString() const override;

private:
  Vec3f m_dxCamera{};
  Vec3f m_dyCamera{};
  
  Float m_cosTotalWidth{};
  Float m_imagePlaneArea{};

  Float m_lensRadiusMM{};
  Float m_zAxialMM{};
  Float m_exposureScale{};
};

PerspectiveCamera::PerspectiveCamera(const CameraTransform& cameraTransform, const Transform4f& camToWorld, const CameraShutter& shutter, FilmBase& film, Float fovDegrees, const Bounds2f& screenWindow, Float lensRadius, Float focalDistance) noexcept : ProjectiveCamera(cameraTransform, camToWorld, shutter, film, Transform4f::perspective(fovDegrees, static_cast<Float>(1e-2), static_cast <Float>(1000.0)), screenWindow, lensRadius, focalDistance)
{

  const PhysicalUnits physicalUnits{};

  m_lensRadiusMM = m_lensRadius * physicalUnits.unitLengthInMM;
  m_zAxialMM = Float{ 1 } * physicalUnits.unitLengthInMM;
  m_exposureScale = (m_lensRadiusMM > 0) ? ((kPi * (m_lensRadiusMM * m_lensRadiusMM)) / (m_zAxialMM * m_zAxialMM)) : Float{ 1 };

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

  // Defocus blur
  if (m_lensRadius > Float{})
  {
    const auto& lensSample{ concentricSampleDisk(cs.pLens) };
    const Point2f pLens{ lensSample[0] * m_lensRadius, lensSample[1] * m_lensRadius };
    originCam = Point3f{ pLens[0], pLens[1], Float{} };

    const Float ft{ m_focalDistance / unitDirCam[2] };
    const Point3f pFocus{ Point3f{} + (unitDirCam * ft) };
    unitDirCam = normalize(pFocus - originCam);
  }

  const Float lerped_t{ lerp(cs.time, m_cameraShutter.shutterOpen, m_cameraShutter.shutterClose) };

  const Ray cameraSpaceRay{ originCam, unitDirCam, lerped_t };
  const Ray renderSpaceRay{ m_cameraTransform.applyRenderFromCamera(cameraSpaceRay) };

  const Float cosineTheta{ std::max(Float{}, unitDirCam[2]) };
  
  // Ae/z^2 factor pending real lenses 
  const Float camWeight{ sqr(sqr(cosineTheta)) * m_exposureScale };

  return CameraRay{ renderSpaceRay, camWeight, m_exposureScale };
}

std::string PerspectiveCamera::toString() const
{
  return "perspective";
}
