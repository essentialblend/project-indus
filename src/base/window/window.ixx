export module window;

import <functional>;
import <future>;
import <vector>;

import core_constructs;
import stats_overlay;
import timer;
import color;

export class SFMLWindow
{
public:
	explicit SFMLWindow() noexcept = default;
    explicit SFMLWindow(const PixelResolution& pixResObj) noexcept : m_windowPixelRes(pixResObj) {}

	void setupWindow();
    [[noreturn]] void displayWindow(StatsOverlay& statsOverlayObj, Timer& timerObj);
    void setupWindowSFMLParams();
    void checkForUpdates();
    void drawGUI(StatsOverlay& statsOverlayObj, Timer& timerObj);
    void processInputEvents(StatsOverlay& statsOverlayObj, Timer& timerObj);
    [[nodiscard]] SFMLWindowProperties& getSFMLWindowProperties() noexcept;
    [[noreturn]] void setResolution(const PixelResolution& windowPixResObj) noexcept;

    void setRenderFrameSingleCoreFunctor(const std::function<void()>& singleCoreFunctor) noexcept;
    void setRenderFrameMultiCoreFunctor(const std::function<void()>& multiCoreFunctor) noexcept;
    void setMultithreadedCheckFunctor(const std::function<bool()>& isMultithreadedCheckFunctor) noexcept;

    void setTextureUpdateCheckFunctor(const std::function<bool()>& texUpdateCheckFunctor) noexcept;
    void setTextureCounterGetterFunctor(const std::function<std::pair<int, int>()>& texUpdateCounterGetter) noexcept;

    void setMainEngineFramebufferGetFunctor(const std::function<std::vector<Color>()>& mainEngineFramebufferGetFunctor) noexcept;
    void setMainRendererCameraPropsGetFunctor(const std::function<CameraProperties()>& mainRendererCameraPropsGetFunctor);

private:
    SFMLWindowProperties m_windowProps{};
    WindowFunctors m_windowFunctors{};
    PixelResolution m_windowPixelRes{};
    std::string m_windowTitle{"indus prelim"};
    std::future<void> m_renderingStatusFuture{};
    bool m_isRendering{ false };
    bool m_needsDrawUpdate{ false };
    int m_texUpdateChunkTracker{ 0 };


    int TEMP_TO_REMOVE{ 0 };
};


