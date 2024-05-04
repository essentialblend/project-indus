import camera;

import <cmath>;

import core_util;

Camera::Camera(const PixelResolution& resObj, const AspectRatio& arObj) noexcept : m_cameraProps{ .camImgPropsObj{.pixelResolutionObj{resObj}, .aspectRatioObj{arObj}} } {}

void Camera::setupCamera()
{
    auto& localPixelResPropsObj{ m_cameraProps.camImgPropsObj.pixelResolutionObj };
    auto& localViewportPropsObj{ m_cameraProps.camViewportPropsObj };
    auto& localARPropsObj{ m_cameraProps.camImgPropsObj.aspectRatioObj };
    auto& localPixDimObj{ m_cameraProps.camPixelDimObj };

    localPixelResPropsObj.heightInPixels = std::max(1, static_cast<int>(localPixelResPropsObj.widthInPixels / (localARPropsObj.widthInAbsVal / localARPropsObj.heightInAbsVal)));

    const auto theta = UDegreesToRadians(m_cameraProps.camVerticalFOV);
    const auto h = std::tan(theta / 2);
    localViewportPropsObj.heightInWorldSpaceUnits = 2 * h * m_cameraProps.focusDist;
    localViewportPropsObj.widthInWorldSpaceUnits = localViewportPropsObj.heightInWorldSpaceUnits * static_cast<double>(localPixelResPropsObj.widthInPixels) / localPixelResPropsObj.heightInPixels;

    localViewportPropsObj.horizontalPixelSpan = localViewportPropsObj.widthInWorldSpaceUnits * m_cameraProps.camU;
    localViewportPropsObj.verticalPixelSpan = localViewportPropsObj.heightInWorldSpaceUnits * -m_cameraProps.camV;

    localPixDimObj.lateralSpanInAbsVal = localViewportPropsObj.horizontalPixelSpan / localPixelResPropsObj.widthInPixels;
    localPixDimObj.verticalSpanInAbsVal = localViewportPropsObj.verticalPixelSpan / localPixelResPropsObj.heightInPixels;
    
    const Point viewportTopLeft{ m_cameraProps.camCenter - (m_cameraProps.focusDist * m_cameraProps.camW) - (localViewportPropsObj.horizontalPixelSpan / 2) - (localViewportPropsObj.verticalPixelSpan / 2) };
    
    localPixDimObj.topLeftmostPixelCenter = viewportTopLeft + (0.5 * (localPixDimObj.lateralSpanInAbsVal + localPixDimObj.verticalSpanInAbsVal));

    // Defocus disk basis vectors.
    const auto defocusRadius{ m_cameraProps.focusDist * std::tan(UDegreesToRadians(m_cameraProps.defocusAngle / 2)) };
    m_cameraProps.defocusDiskU = m_cameraProps.camU * defocusRadius;
    m_cameraProps.defocusDiskV = m_cameraProps.camV * defocusRadius;
}

void Camera::setViewportHeight(double vpHeightInWorldSpace) noexcept
{
    m_cameraProps.camViewportPropsObj.heightInWorldSpaceUnits = vpHeightInWorldSpace;
}

void Camera::setCameraCenter(const Point& centerPointInWorldSpace) noexcept
{
    m_cameraProps.camCenter = centerPointInWorldSpace;
}

CameraProperties Camera::getCameraProperties() const noexcept
{
    return m_cameraProps;
}