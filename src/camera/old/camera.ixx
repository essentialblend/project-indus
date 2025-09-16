export module camera;

import <array>;

import core_constructs;
import constructs;
import vec3;
import ray;
import point;

export class Camera
{
public:
	explicit Camera() noexcept = default;
    explicit Camera(const PixelResolution& resObj, const AspectRatio& arObj) noexcept;
	
    void setupCamera();

    void setViewportHeight(double vpHeightInWorldSpace = 2.0) noexcept;
    void setPixelDimensions(const PixelDimension& pixDimObj) noexcept;
    void setCameraCenter(const Point3& centerPointInWorldSpace = Point3{}) noexcept;
    Ray generateRay(int pixelX, int pixelY, Point2 uPixel, Point2 uLens) const noexcept;

    [[nodiscard]] CameraProperties getCameraProperties() const noexcept;

private:
    CameraProperties m_cameraProps{};
};


