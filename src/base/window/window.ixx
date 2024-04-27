export module window;

import <functional>;
import <future>;

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
    void displayWindow(StatsOverlay& statsOverlayObj, Timer& timerObj);
    void startPDHQuery(PDHVariables& pdhVars);
    void setupWindowSFMLParams();
    void checkForUpdates(StatsOverlay& statsOverlayObj, Timer& timerObj, PDHVariables& pdhVars);
    void updateRenderingStatus(Timer& timerObj, StatsOverlay& statsOverlayObj);
    void updateTextureForDisplay();
    void drawGUI(StatsOverlay& statsOverlayObj, const Timer& timerObj);
    void processInputEvents(StatsOverlay& statsOverlayObj, Timer& timerObj);
    [[nodiscard]] SFMLWindowProperties& getSFMLWindowProperties() noexcept;
    void setResolution(const PixelResolution& windowPixResObj) noexcept;

    void setRenderFrameMultiCoreFunctor(const std::function<void()>& multiCoreFunctor) noexcept;
    void setMultithreadedCheckFunctor(const std::function<bool()>& isMultithreadedCheckFunctor) noexcept;

    void setTextureUpdateCheckFunctor(const std::function<bool()>& texUpdateCheckFunctor) noexcept;

    void setMainEngineFramebufferGetFunctor(const std::function<std::vector<Color>()>& mainEngineFramebufferGetFunctor) noexcept;
    void setMainRendererCameraPropsGetFunctor(const std::function<CameraProperties()>& mainRendererCameraPropsGetFunctor);
    void setRenderCompleteStatusGetFunctor(const std::function<bool()>& renderCompleteStatusFunctor) noexcept;
    void setTextureUpdateRateGetFunctor(const std::function<int()>& texUpdateRateFunctor) noexcept;

private:
    SFMLWindowProperties m_windowProps{};
    WindowFunctors m_windowFunctors{};
    PDHVariables m_pdhVars{};
    PixelResolution m_windowPixelRes{};
    Timer m_cpuUsagePDHTimer{};

    std::string m_windowTitle{"indus prelim"};
    std::future<void> m_renderingStatusFuture{};
    bool m_needsDrawUpdate{ false };
    bool m_isRendering{ false };
    int m_texUpdateChunkTracker{ 0 };
    
    [[nodiscard]] double getCPUUsageWithPDH(PDHVariables& pdhVars);
    void updatePDHOverlayPeriodic(StatsOverlay& statsOverlayObj, PDHVariables& pdhVars);
};


