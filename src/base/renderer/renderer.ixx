export module renderer;

import <iostream>;
import <vector>;
import <print>;
import <span>;
import <functional>;

import core_constructs;
import color;
import ray;
import camera;
import vec3;
import window;
import stats_overlay;
import timer;

import <SFML/Graphics.hpp>;

export class Renderer
{
public:
	explicit Renderer() noexcept = default;
    
    void setupRenderer(const PixelResolution& pixResObj, const AspectRatio& aspectRatioObj);
    void renderFrameSingleCore(const PixelResolution& pixResObj, const PixelDimension& pixDimObj, const Point& camCenter, std::vector<Color>& primaryPixelBuffer);
    void renderFrameMultiCore(const PixelResolution& pixResObj, const PixelDimension& pixDimObj, const Point& camCenter, std::vector<Color>& primaryPixelBuffer);

    void setRendererFunctors(const RendererSFMLFunctors& rendererFuncObj) noexcept
    {
        m_rendererFunctors = rendererFuncObj;
    }

    void setThreadingMode(bool isMultithreaded) noexcept
    {
        m_isMultithreaded = isMultithreaded;
    }

    [[nodiscard]] bool getThreadingMode() const noexcept
    {
        return m_isMultithreaded;
    }
    
    [[nodiscard]] CameraProperties getRendererCameraProps() const noexcept;

    

private:   
    Camera m_mainCamera{};
    RendererSFMLFunctors m_rendererFunctors{};
    bool m_isMultithreaded{ true };
    [[nodiscard]] static Color computeRayColor(const Ray& inputRay);
    void renderPixelRowThreadPoolTask(const PixelResolution& pixResObj, const PixelDimension& pixDimObj, const Point& camCenter, std::vector<Color>& primaryPixelBuffer, int columnPixelForNewRow) const;
};




