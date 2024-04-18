export module window;

import <functional>;

import core_constructs;
import stats_overlay;
import timer;

export class SFMLWindow
{
public:
	explicit SFMLWindow() noexcept = default;
    explicit SFMLWindow(const PixelResolution& pixResObj) noexcept : m_windowPixelRes(pixResObj) {}

	void setupWindow();
    void displayWindow(StatsOverlay& statsOverlayObj, const Timer& timerObj);

    SFMLWindowProperties& getSFMLWindowProperties() noexcept;

    void setResolution(const PixelResolution& windowPixResObj) noexcept
    {
        m_windowPixelRes = windowPixResObj;
    }

    void setRenderFrameFunctor(const std::function<void()>& renderFrameFunctor) noexcept
	{
		m_renderFrameFunctor = renderFrameFunctor;
	}

private:
    SFMLWindowProperties m_windowProps{};
    std::function<void()> m_renderFrameFunctor{};
    PixelResolution m_windowPixelRes{};
    std::string m_windowTitle{"indus prelim"};
};


