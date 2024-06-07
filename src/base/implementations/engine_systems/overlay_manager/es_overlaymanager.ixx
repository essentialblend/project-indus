export module es_overlaymanager;

import i_enginesystem;
import ri_singleton;
import eu_overlay;

import <memory>;
import <sfml/Graphics.hpp>;


export class ESOverlayManager final : public IEngineSystem, public RISingleton<ESOverlayManager>
{
public:
    void initializeEntity([[maybe_unused]] const std::vector<std::any>& args = {}) override;
    void addOverlay(std::shared_ptr<EUOverlay> overlay);

	[[nodiscard]] std::vector<std::shared_ptr<EUOverlay>>& getOverlaysMutable() noexcept;

private:
    std::vector<std::shared_ptr<EUOverlay>> m_overlayList{};

    // Private as this is a singleton class.
    explicit ESOverlayManager() noexcept = default;
    ESOverlayManager(const ESOverlayManager&) = delete;
    ESOverlayManager& operator=(const ESOverlayManager&) = delete;
    ESOverlayManager(ESOverlayManager&&) noexcept = delete;
    ESOverlayManager& operator=(ESOverlayManager&&) noexcept = delete;
    ~ESOverlayManager() noexcept = default;

    friend class RISingleton<ESOverlayManager>;
};
