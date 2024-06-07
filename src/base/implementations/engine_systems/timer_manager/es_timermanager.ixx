export module es_timermanager;

import <vector>;
import <string>;
import <memory>;

import i_enginesystem;
import eu_timer;

import ri_manageable;
import ri_singleton;

export class ESTimerManager final : public IEngineSystem, public RIManageable, public RISingleton<ESTimerManager>
{
public:

    void initializeEntity([[maybe_unused]] const std::vector<std::any>& args = {}) override;
    void manageEntityResources() override;

    void addTimer(std::string_view timerName);
    const EUTimer& findTimer(std::string_view) const;
    EUTimer& findTimerMutable(std::string_view);
    void removeTimer(std::string_view);

    std::vector<std::shared_ptr<EUTimer>>& getTimersMutable() noexcept;

private:
    std::vector<std::shared_ptr<EUTimer>> m_masterTimerList{};

    // Private as this is a singleton class.
    explicit ESTimerManager() noexcept = default;
    ESTimerManager(const ESTimerManager&) = delete;
    ESTimerManager& operator=(const ESTimerManager&) = delete;
    ESTimerManager(ESTimerManager&&) noexcept = delete;
    ESTimerManager& operator=(ESTimerManager&&) noexcept = delete;
	~ESTimerManager() = default;

    friend class RISingleton<ESTimerManager>;
};


