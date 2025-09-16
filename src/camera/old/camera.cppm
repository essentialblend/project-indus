import camera;

import <cmath>;

import core_util;
import core_sampling_util;
import ray;
import constructs;
import core_constructs;

Camera::Camera(const PixelResolution& resObj, const AspectRatio& arObj) noexcept : m_cameraProps{ .camImgPropsObj{resObj, arObj} } {}

void Camera::setupCamera()
{
    auto& localPixelResPropsObj{ m_cameraProps.camImgPropsObj.pixelResolutionObj };
    auto& localViewportPropsObj{ m_cameraProps.camViewportPropsObj };
    auto& localARPropsObj{ m_cameraProps.camImgPropsObj.aspectRatioObj };
    auto& localPixDimObj{ m_cameraProps.camPixelDimObj };

    // Calculate pixel resolution height using resolution width and aspect ratio
    localPixelResPropsObj.heightInPixels = std::max(1, static_cast<int>(localPixelResPropsObj.widthInPixels / (localARPropsObj.widthAbsVal / localARPropsObj.heightAbsVal)));

    // Take vertical field of view, split into two right triangles
    const auto theta = UDegreesToRadians(m_cameraProps.camVerticalFOV);
    const auto h = std::tan(theta / 2);
    
    // Determine viewport dimensions in proportion to focus distance and h
    localViewportPropsObj.heightWorldSpaceUnits = (2 * h) * m_cameraProps.camDefocusPropsObj.focusDist;
    
    localViewportPropsObj.widthWorldSpaceUnits = localViewportPropsObj.heightWorldSpaceUnits * (static_cast<double>(localPixelResPropsObj.widthInPixels) / localPixelResPropsObj.heightInPixels);

    // Calculate the horizontal and vertical span of one pixel
    localViewportPropsObj.horPixelSpanVec = localViewportPropsObj.widthWorldSpaceUnits * m_cameraProps.camU;
    
    localViewportPropsObj.vertPixelSpanVec = localViewportPropsObj.heightWorldSpaceUnits * -m_cameraProps.camV;

    localPixDimObj.lateralSpanVec = localViewportPropsObj.horPixelSpanVec / localPixelResPropsObj.widthInPixels;
    
    localPixDimObj.vertSpanVec = localViewportPropsObj.vertPixelSpanVec / localPixelResPropsObj.heightInPixels;
     
    // Determine the top left point of the viewport
    const Point3 viewportTopLeft{ m_cameraProps.camCenter - (m_cameraProps.camDefocusPropsObj.focusDist * m_cameraProps.camW) - (localViewportPropsObj.horPixelSpanVec / 2) - (localViewportPropsObj.vertPixelSpanVec / 2) };
    
    // Use viewport's top left point to determine the center of the top-leftmost pixel
    localPixDimObj.topLeftPixCenter = viewportTopLeft + (0.5 * (localPixDimObj.lateralSpanVec + localPixDimObj.vertSpanVec));

    // Defocus disk basis vectors
    const auto defocusRadius{ m_cameraProps.camDefocusPropsObj.focusDist * std::tan(UDegreesToRadians(m_cameraProps.camDefocusPropsObj.defocusAngle / 2)) };
    m_cameraProps.camDefocusPropsObj.defocusDiskU = m_cameraProps.camU * defocusRadius;
    m_cameraProps.camDefocusPropsObj.defocusDiskV = m_cameraProps.camV * defocusRadius;
}

Ray Camera::generateRay(int pixelX, int pixelY, Point2 uPixel, Point2 uLens) const noexcept
{
  const auto& pixProps{ m_cameraProps.camPixelDimObj };
  const auto& defocusProps{ m_cameraProps.camDefocusPropsObj };

  Point3 samplePoint{ pixProps.topLeftPixCenter + ((pixelX + uPixel[0]) * pixProps.lateralSpanVec) + ((pixelY + uPixel[1]) * pixProps.vertSpanVec)};

  Point2 lensSample{ concentricSampleDisk(uLens) };

  Point3 randomRayOriginOnDisk = m_cameraProps.camCenter + (lensSample[0] * defocusProps.defocusDiskU) + (lensSample[1] * defocusProps.defocusDiskV);

  Point3 rayOrigin = (defocusProps.defocusAngle <= 0.0) ? m_cameraProps.camCenter : randomRayOriginOnDisk;

  Vec3 rayDir{ getUnit(samplePoint - rayOrigin) };

  return Ray(rayOrigin, rayDir);
}

void Camera::setViewportHeight(double vpHeightInWorldSpace) noexcept
{
    m_cameraProps.camViewportPropsObj.heightWorldSpaceUnits = vpHeightInWorldSpace;
}

void Camera::setCameraCenter(const Point3& centerPointInWorldSpace) noexcept
{
    m_cameraProps.camCenter = centerPointInWorldSpace;
}

void Camera::setPixelDimensions(const PixelDimension& pixDimObj) noexcept
{
    m_cameraProps.camPixelDimObj = pixDimObj;
}

CameraProperties Camera::getCameraProperties() const noexcept
{
    return m_cameraProps;
}