export module renderer;

import <iostream>;
import <vector>;
import <print>;
import <span>;
import <functional>;
import <memory>;
import <atomic>;
import <future>;
import <latch>;
import <mutex>;

import core_constructs;
import ray;
import camera;
import vec3;
import stats_overlay;
import u_timer;
import threadpool;
import world_object;
import sampler;

export class Renderer
{
public:
	explicit Renderer() noexcept = default;
    
    void setupRenderer(const PixelResolution& pixResObj, const AspectRatio& aspectRatioObj);
    void renderFrame(std::vector<ColorRGB>& mainFramebuffer, const WorldObject& mainWorld, bool isMultithreaded);

    // Temporary LiTest
    void renderLiTestFrame(std::vector<ColorRGB>& mainFramebuffer, const WorldObject& mainWorld, bool isMultithreaded);

    void setRendererSFMLFunctors(const RendererSFMLFunctors& rendererFuncObj) noexcept;
    void setThreadingMode(bool isMultithreaded) noexcept;
    void setSPP(int spp) noexcept;
    void setRayBounceDepth(int rayBounceDepth) noexcept;

    [[nodiscard]] bool getRenderCompleteStatus() noexcept;
    [[nodiscard]] bool getThreadingMode() const noexcept;
    [[nodiscard]] int getTexUpdateRate() const noexcept;
    [[nodiscard]] GaussianKernelProperties getGaussianKernelProps() const noexcept;
    [[nodiscard]] CameraProperties getRendererCameraProps() const noexcept;
    
    [[nodiscard]] bool checkForDrawUpdate();

private:   
    Camera m_mainCamera{};
    RendererSFMLFunctors m_rendererFunctors{};
    MT_ThreadPool m_renderThreadPool{};
    GaussianKernelProperties m_gaussianKernelProps{};
    std::unique_ptr<Sampler> m_rendererSampler{};

    bool m_isMultithreaded{ false };
    std::mutex m_framebufferMutex{};

    bool m_isRenderComplete{ false };
    std::vector<std::future<void>> m_mainRenderingPassFutureVec{};

    std::unique_ptr<std::latch> m_texUpdateLatch{};
    int m_texUpdateRateOut{ 50 };
    int m_maxRayBounceDepth{ 10 };

    // Sampler vars
    int m_samplerSeed{ 1337 };
    int m_SPP{};
    bool m_useRussianRoulette{ true };
    
    // Debug variables
    inline static thread_local bool m_samplesRejected = false;
    std::atomic<std::uint64_t> m_pixelsFlagged{};
    std::atomic<std::uint64_t> m_pathsTerminated{};
    std::atomic<std::uint64_t> m_pathsSurvived{};
    std::atomic<std::uint64_t> m_totalBounces{};
    std::atomic<std::uint64_t> m_totalPaths{};
    bool m_diagPrinted{ false };

    UTimer m_mainRenderTimer{};

    [[nodiscard]] ColorRGB Li(const Ray& unitInputRay, const WorldObject& mainWorld, int maxRayBounceDepth, Sampler& sampler);
    [[nodiscard]] ColorRGB getBackgroundGradient(const Ray& inputRay);
    void renderPixelRowThreadPoolTask(int currentColumnCount, std::vector<ColorRGB>& primaryPixelBuffer, const WorldObject& mainWorld);
    static bool areFuturesReadyInRange(int startOffset, std::span<std::future<void>> iterableFutureContainer, int& optExistingTracker);
    void setupGaussianKernel(PixelDimension& localPixDimObj);
};

 


