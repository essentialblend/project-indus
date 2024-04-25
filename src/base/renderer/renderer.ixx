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

import <SFML/Graphics.hpp>;

export class Renderer
{
public:
	explicit Renderer() noexcept = default;
    
    void setupRenderer(const PixelResolution& pixResObj, const AspectRatio& aspectRatioObj);
    [[deprecated]] void renderFrameSingleCore(const PixelResolution& pixResObj, const PixelDimension& pixDimObj, const Point& camCenter, std::vector<Color>& primaryPixelBuffer);
    void renderFrameMultiCore(std::vector<Color>& mainFramebuffer);

    void setRendererSFMLFunctors(const RendererSFMLFunctors& rendererFuncObj) noexcept;
    void setThreadingMode(bool isMultithreaded) noexcept;

    [[nodiscard]] bool getThreadingMode() const noexcept;
    [[nodiscard]] std::pair<int, int> getTextureUpdateCounters() const;
    [[nodiscard]] CameraProperties getRendererCameraProps() const noexcept;

    bool checkForDrawUpdate();

private:   
    Camera m_mainCamera{};
    RendererSFMLFunctors m_rendererFunctors{};
    bool m_isMultithreaded{ true };

    MT_ThreadPool m_renderThreadPool{};
    std::vector<std::future<void>> m_texUpdateFutureVec{};
    std::unique_ptr<std::latch> m_texUpdateLatch{};
    int m_currChunkForTexUpdate{ 0 };
    int m_texUpdateRate{ 50 };

    [[nodiscard]] static Color computeRayColor(const Ray& inputRay);
    static const Color getBackgroundGradient(const Ray& inputRay);
    void renderPixelRowThreadPoolTask(int currentColumnCount, std::vector<Color>& primaryPixelBuffer);

    static double tempHitSphere(const Point& sphereCenter, double sphereRadius, const Ray& inputRay);
};




