export module renderer;

import <iostream>;
import <vector>;
import <print>;
import <span>;
import <functional>;
import <atomic>;
import <future>;
import <latch>;
import <mutex>;

import core_constructs;
import color;
import ray;
import camera;
import vec3;
import window;
import stats_overlay;
import timer;
import threadpool;
import world_object;

import <SFML/Graphics.hpp>;

export class Renderer
{
public:
	explicit Renderer() noexcept = default;
    
    void setupRenderer(const PixelResolution& pixResObj, const AspectRatio& aspectRatioObj);
    void renderFrameMultiCore(std::vector<Color>& mainFramebuffer, const WorldObject& mainWorld);

    void setRendererSFMLFunctors(const RendererSFMLFunctors& rendererFuncObj) noexcept;
    void setThreadingMode(bool isMultithreaded) noexcept;

    [[nodiscard]] bool getRenderCompleteStatus() noexcept;
    [[nodiscard]] bool getThreadingMode() const noexcept;
    [[nodiscard]] CameraProperties getRendererCameraProps() const noexcept;
    [[nodiscard]] int getTexUpdateRate() const noexcept;
    
    bool checkForDrawUpdate();

private:   
    Camera m_mainCamera{};
    RendererSFMLFunctors m_rendererFunctors{};
    bool m_isMultithreaded{ true };

    MT_ThreadPool m_renderThreadPool{};
    std::vector<std::future<void>> m_renderingStatusFutureVec{};
    std::unique_ptr<std::latch> m_texUpdateLatch{};
    int m_texUpdateRate{ 50 };
    int m_samplesPerPixel{ 2500 };
    int m_maxRayBounceDepth{ 250 };

    [[nodiscard]] Ray getRayForPixel(int i, int currentRowCount) const noexcept;
    [[nodiscard]] static Color computeRayColor(const Ray& inputRay, const WorldObject& mainWorld, int maxRayBounceDepth);
    static const Color getBackgroundGradient(const Ray& inputRay);
    void renderPixelRowThreadPoolTask(int currentColumnCount, std::vector<Color>& primaryPixelBuffer, const WorldObject& mainWorld);
};




