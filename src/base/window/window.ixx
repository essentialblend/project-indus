export module window;

import <functional>;
import <future>;

import core_constructs;
import stats_overlay;
import timer;

export class SFMLWindow
{
public:
	explicit SFMLWindow() noexcept = default;
    explicit SFMLWindow(const PixelResolution& pixResObj) noexcept : m_windowPixelRes(pixResObj) {}

	void setupWindow();
    [[noreturn]] void displayWindow(StatsOverlay& statsOverlayObj, Timer& timerObj);

    void drawGUI(StatsOverlay& statsOverlayObj, Timer& timerObj);

    void processInputEvents(StatsOverlay& statsOverlayObj, Timer& timerObj);

    [[nodiscard]] SFMLWindowProperties& getSFMLWindowProperties() noexcept;

    [[noreturn]] void setResolution(const PixelResolution& windowPixResObj) noexcept
    {
        m_windowPixelRes = windowPixResObj;
    }

    void setRenderFrameSingleCoreFunctor(const std::function<void()>& singleCoreFunctor) noexcept
	{
		m_renderFrameSingleCoreFunctor = singleCoreFunctor;
	}

    void setRenderFrameMultiCoreFunctor(const std::function<void()>& singleCoreFunctor) noexcept
	{
		m_renderFrameMultiCoreFunctor = singleCoreFunctor;
	}

    void setMultithreadedCheckFunctor(const std::function<bool()>& isMultithreadedCheckFunctor) noexcept
    {
        m_isMultithreadedFunctor = isMultithreadedCheckFunctor;
    }

private:
    SFMLWindowProperties m_windowProps{};
    std::function<void()> m_renderFrameSingleCoreFunctor{};
    std::function<void()> m_renderFrameMultiCoreFunctor{};
    std::function<bool()> m_isMultithreadedFunctor{};
    PixelResolution m_windowPixelRes{};
    std::string m_windowTitle{"indus prelim"};
    std::future<void> m_renderingStatusFuture{};
};


