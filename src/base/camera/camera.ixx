export module camera;

import <array>;

import core_constructs;
import vec3;

export class Camera
{
public:
	explicit Camera() noexcept = default;
    explicit Camera(const PixelResolution& resObj, const AspectRatio& arObj) : m_cameraProps{ .camImgPropsObj{.pixelResolutionObj{resObj}, .aspectRatioObj{arObj}} } {}
	
	void setupCamera();

    void setViewportHeight(double vpHeightInWorldSpace = 2.0) noexcept
    {
        m_cameraProps.camViewportPropsObj.heightInWorldSpaceUnits = vpHeightInWorldSpace;
    }

    void setFocalLength(const Vec3& focalLengthInCartesianZ) noexcept
    {
        m_cameraProps.camFocalLength = focalLengthInCartesianZ;
    }

    void setCameraCenter(const Point& centerPointInWorldSpace = Point(0)) noexcept
    {
        m_cameraProps.camCenter = centerPointInWorldSpace;
    }

    CameraProperties getCameraProperties() const noexcept
    {
        return m_cameraProps;
    }

private:
    CameraProperties m_cameraProps{};
};

void Camera::setupCamera() 
{
    const auto localPixelResProps { m_cameraProps.camImgPropsObj.pixelResolutionObj };
    const auto localViewportProps{ m_cameraProps.camViewportPropsObj };
    const auto localARProps{ m_cameraProps.camImgPropsObj.aspectRatioObj };

    m_cameraProps.camImgPropsObj.pixelResolutionObj.heightInPixels = std::max(1, static_cast<int>(localPixelResProps.widthInPixels / (localARProps.widthInAbsVal / localARProps.heightInAbsVal)));

    m_cameraProps.camViewportPropsObj.widthInWorldSpaceUnits = localViewportProps.heightInWorldSpaceUnits * static_cast<double>(localPixelResProps.widthInPixels) / m_cameraProps.camImgPropsObj.pixelResolutionObj.heightInPixels;
    
    m_cameraProps.camViewportPropsObj.horizontalPixelSpan = Vec3(m_cameraProps.camViewportPropsObj.widthInWorldSpaceUnits, 0, 0);
    
    m_cameraProps.camViewportPropsObj.verticalPixelSpan = Vec3(0, -localViewportProps.heightInWorldSpaceUnits, 0);

    const Point viewportTopLeft{ m_cameraProps.camCenter - m_cameraProps.camFocalLength - (m_cameraProps.camViewportPropsObj.horizontalPixelSpan / 2) - (m_cameraProps.camViewportPropsObj.verticalPixelSpan / 2) };

    m_cameraProps.camPixelDimObj.lateralSpanInAbsVal = m_cameraProps.camViewportPropsObj.horizontalPixelSpan / localPixelResProps.widthInPixels;
    m_cameraProps.camPixelDimObj.verticalSpanInAbsVal = m_cameraProps.camViewportPropsObj.verticalPixelSpan / m_cameraProps.camImgPropsObj.pixelResolutionObj.heightInPixels;
    m_cameraProps.camPixelDimObj.pixelCenter = viewportTopLeft + (0.5 * (m_cameraProps.camPixelDimObj.lateralSpanInAbsVal + m_cameraProps.camPixelDimObj.verticalSpanInAbsVal));
}
