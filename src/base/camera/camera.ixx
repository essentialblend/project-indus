export module camera;

import <array>;

import core_constructs;
import vec3;

export class Camera
{
public:
	explicit Camera() noexcept = default;
    explicit Camera(const PixelResolution& resObj, const AspectRatio& arObj) noexcept;
	
    [[noreturn]] void setupCamera();

    [[noreturn]] void setViewportHeight(double vpHeightInWorldSpace = 2.0) noexcept;
    [[noreturn]] void setFocalLength(const Vec3& focalLengthInCartesianZ) noexcept;
    [[noreturn]] void setCameraCenter(const Point& centerPointInWorldSpace = Point(0)) noexcept;

    [[nodiscard]] CameraProperties getCameraProperties() const noexcept;

private:
    CameraProperties m_cameraProps{};
};


