import camera;

import <cmath>;

import core_util;

Camera::Camera(const PixelResolution& resObj, const AspectRatio& arObj) noexcept : m_cameraProps{ .camImgPropsObj{resObj, arObj} } {}

void Camera::setupCamera()
{
    auto& localPixelResPropsObj{ m_cameraProps.camImgPropsObj.pixelResolutionObj };
    auto& localViewportPropsObj{ m_cameraProps.camViewportPropsObj };
    auto& localARPropsObj{ m_cameraProps.camImgPropsObj.aspectRatioObj };
    auto& localPixDimObj{ m_cameraProps.camPixelDimObj };

    localPixelResPropsObj.heightInPixels = std::max(1, static_cast<int>(localPixelResPropsObj.widthInPixels / (localARPropsObj.widthAbsVal / localARPropsObj.heightAbsVal)));

    const auto theta = UDegreesToRadians(m_cameraProps.camVerticalFOV);
    const auto h = std::tan(theta / 2);
    localViewportPropsObj.heightWorldSpaceUnits = 2 * h * m_cameraProps.camDefocusPropsObj.focusDist;
    localViewportPropsObj.widthWorldSpaceUnits = localViewportPropsObj.heightWorldSpaceUnits * static_cast<double>(localPixelResPropsObj.widthInPixels) / localPixelResPropsObj.heightInPixels;

    localViewportPropsObj.horPixelSpanAbsVal = localViewportPropsObj.widthWorldSpaceUnits * m_cameraProps.camU;
    localViewportPropsObj.vertPixelSpanAbsVal = localViewportPropsObj.heightWorldSpaceUnits * -m_cameraProps.camV;

    localPixDimObj.lateralSpanAbsVal = localViewportPropsObj.horPixelSpanAbsVal / localPixelResPropsObj.widthInPixels;
    localPixDimObj.vertSpanAbsVal = localViewportPropsObj.vertPixelSpanAbsVal / localPixelResPropsObj.heightInPixels;
    
    const Point viewportTopLeft{ m_cameraProps.camCenter - (m_cameraProps.camDefocusPropsObj.focusDist * m_cameraProps.camW) - (localViewportPropsObj.horPixelSpanAbsVal / 2) - (localViewportPropsObj.vertPixelSpanAbsVal / 2) };
    
    localPixDimObj.topLeftPixCenter = viewportTopLeft + (0.5 * (localPixDimObj.lateralSpanAbsVal + localPixDimObj.vertSpanAbsVal));

    // Defocus disk basis vectors.
    const auto defocusRadius{ m_cameraProps.camDefocusPropsObj.focusDist * std::tan(UDegreesToRadians(m_cameraProps.camDefocusPropsObj.defocusAngle / 2)) };
    m_cameraProps.camDefocusPropsObj.defocusDiskU = m_cameraProps.camU * defocusRadius;
    m_cameraProps.camDefocusPropsObj.defocusDiskV = m_cameraProps.camV * defocusRadius;
}

void Camera::setViewportHeight(double vpHeightInWorldSpace) noexcept
{
    m_cameraProps.camViewportPropsObj.heightWorldSpaceUnits = vpHeightInWorldSpace;
}

void Camera::setCameraCenter(const Point& centerPointInWorldSpace) noexcept
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