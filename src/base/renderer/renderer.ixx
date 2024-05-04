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
    void samplesCollectionRenderPassMultiCore(const WorldObject& mainWorld);

    void setRendererSFMLFunctors(const RendererSFMLFunctors& rendererFuncObj) noexcept;
    void setThreadingMode(bool isMultithreaded) noexcept;

    [[nodiscard]] bool getSampleCollectionPassCompleteStatus() noexcept;
    [[nodiscard]] bool getRenderCompleteStatus() noexcept;
    [[nodiscard]] bool getThreadingMode() const noexcept;
    [[nodiscard]] CameraProperties getRendererCameraProps() const noexcept;
    [[nodiscard]] int getTexUpdateRate() const noexcept;
    
    [[nodiscard]] bool checkForDrawUpdate();

private:   
    Camera m_mainCamera{};
    RendererSFMLFunctors m_rendererFunctors{};
    std::vector<PixelSamples> m_pixelSamplesBuffer{};
    bool m_isMultithreaded{ true };
    std::mutex m_framebufferMutex{};

    MT_ThreadPool m_renderThreadPool{};
    std::vector<std::future<void>> m_gatherPixelSamplesPassFutureVec{};
    std::vector<std::future<void>> m_mainRenderingPassFutureVec{};
    
    std::unique_ptr<std::latch> m_pixelSamplesPassLatch{};
    std::unique_ptr<std::latch> m_texUpdateLatch{};
    int m_texUpdateRate{ 50 };
    int m_samplesPerPixel{ 25 };
    int m_sppSqrtFloored{ static_cast<int>(std::floor(std::sqrt(m_samplesPerPixel))) };
    int m_maxRayBounceDepth{ 25 };

    [[nodiscard]] Ray getRayForPixel(int i, int currentRowCount) const noexcept;
    [[nodiscard]] Ray getStratifiedRayForPixel(int i, int currentRowCount, int subPixelGridU, int subPixelGridV, Point& currentSamplePoint) const noexcept;
    [[nodiscard]] static Color computeRayColor(const Ray& inputRay, const WorldObject& mainWorld, int maxRayBounceDepth);
    static const Color getBackgroundGradient(const Ray& inputRay);
    void renderPixelRowThreadPoolTask(int currentColumnCount, std::vector<Color>& primaryPixelBuffer, const WorldObject& mainWorld);
    void renderPixelRowThreadPoolTaskGaussian(int currentColumnCount, std::vector<Color>& primaryPixelBuffer);
    void collectPixelSamplesRowThreadPoolTask(int currentRowCount, const WorldObject& mainWorld);
};




