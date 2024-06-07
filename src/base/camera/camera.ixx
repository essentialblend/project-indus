export module camera;

import <array>;

import core_constructs;
import vec3;

export class Camera
{
public:
	explicit Camera() noexcept = default;
    explicit Camera(const PixelResolution& resObj, const AspectRatio& arObj) noexcept;
	
    void setupCamera();

    void setViewportHeight(double vpHeightInWorldSpace = 2.0) noexcept;
    void setPixelDimensions(const PixelDimensionOLD& pixDimObj) noexcept;
    void setCameraCenter(const PointOLD& centerPointInWorldSpace = PointOLD(0)) noexcept;

    [[nodiscard]] CameraProperties getCameraProperties() const noexcept;

private:
    CameraProperties m_cameraProps{};
};


