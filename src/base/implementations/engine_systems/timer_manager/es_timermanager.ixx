export module es_timermanager;

import <vector>;
import <string>;

import i_enginesystem;
import u_timer;

import ri_manageable;

export class ESTimerManager : public IEngineSystem, public RIManageable
{
public:
    static ESTimerManager& getInstance() noexcept;

    void initializeEntity() override;
    void manageEntityResources() override;

    void addTimer(const UTimer&);
    const UTimer& findTimer(std::string_view) const;
    UTimer& findTimerMutable(std::string_view);
    void removeTimer(std::string_view);

private:
    std::vector<UTimer> m_masterTimerList{};

    // Private as this is a singleton class.
    explicit ESTimerManager() noexcept = default;
    ESTimerManager(const ESTimerManager&) = delete;
    ESTimerManager& operator=(const ESTimerManager&) = delete;
    ESTimerManager(ESTimerManager&&) noexcept = delete;
    ESTimerManager& operator=(ESTimerManager&&) noexcept = delete;
	~ESTimerManager() = default;
};


