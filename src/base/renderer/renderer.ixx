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
    void renderFrameMultiCoreGaussian(std::vector<std::unique_ptr<IColor>>& mainFramebuffer, const WorldObject& mainWorld);

    void setRendererSFMLFunctors(const RendererSFMLFunctors& rendererFuncObj) noexcept;
    void setThreadingMode(bool isMultithreaded) noexcept;

    [[nodiscard]] bool getRenderCompleteStatus() noexcept;
    [[nodiscard]] bool getThreadingMode() const noexcept;
    [[nodiscard]] CameraProperties getRendererCameraProps() const noexcept;
    [[nodiscard]] int getTexUpdateRate() const noexcept;
    
    [[nodiscard]] bool checkForDrawUpdate();

private:   
    Camera m_mainCamera{};
    RendererSFMLFunctors m_rendererFunctors{};
    MT_ThreadPool m_renderThreadPool{};
    GaussianKernelProperties m_gaussianKernelProps{};

    bool m_isMultithreaded{ true };
    std::mutex m_framebufferMutex{};

    bool m_isRenderComplete{ false };
    std::vector<std::future<void>> m_mainRenderingPassFutureVec{};

    std::unique_ptr<std::latch> m_texUpdateLatch{};
    int m_texUpdateRateOut{ 50 };
    int m_samplesPerPixel{ 5 };
    int m_sppSqrtCeil{ static_cast<int>(std::ceil(std::sqrt(m_samplesPerPixel))) };
    int m_maxRayBounceDepth{ 50 };
    std::string m_renderColorType{ "ColorRGB" };

    [[nodiscard]] Ray getStratifiedRayForPixel(int i, int currentRowCount, int subPixelGridU, int subPixelGridV, Point& currentSamplePoint) const noexcept;
    [[nodiscard]] std::unique_ptr<const IColor> computeRayColor(const Ray& inputRay, const WorldObject& mainWorld, int maxRayBounceDepth);
    std::unique_ptr<const IColor> getBackgroundGradient(const Ray& inputRay);
    void renderPixelRowThreadPoolTaskGaussian(int currentColumnCount, std::vector<std::unique_ptr<IColor>>& primaryPixelBuffer, const WorldObject& mainWorld);
    static bool areFuturesReadyInRange(int startOffset, std::span<std::future<void>> iterableFutureContainer, int& optExistingTracker);
    void setupGaussianKernel(PixelDimension& localPixDimObj);
    std::unique_ptr<IColor> createDerivedTypeUniquePtr(const std::string& colorType, const Vec3& value) const;
};




