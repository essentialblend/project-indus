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
    void renderFrame(const PixelResolution& pixResObj, const PixelDimension& pixDimObj, const Point& camCenter, std::vector<Color>& primaryPixelBuffer);

    void setRendererFunctors(const RendererFunctors& rendererFuncObj) noexcept
    {
        m_rendererFunctors = rendererFuncObj;
    }

    CameraProperties getRendererCameraProps() const noexcept;

private:   
    Camera m_mainCamera{};
    RendererFunctors m_rendererFunctors{};
    [[nodiscard]] Color computeRayColor(const Ray& inputRay) const;
};




