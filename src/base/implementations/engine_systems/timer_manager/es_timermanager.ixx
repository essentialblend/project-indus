export module es_timermanager;

import <vector>;
import <string>;

import i_enginesystem;
import u_timer;

export class ESTimerManager : public IEngineSystem
{
public:

    virtual void initializeEntity() override;

private:
    std::vector<UTimer> m_activeTimers{};

    // Private as this is a singleton class.
    explicit ESTimerManager() noexcept = default;
    ESTimerManager(const ESTimerManager&) = delete;
    ESTimerManager& operator=(const ESTimerManager&) = delete;
    ESTimerManager(ESTimerManager&&) noexcept = default;
    ESTimerManager& operator=(ESTimerManager&&) noexcept = default;
	~ESTimerManager() = default;

    const UTimer& findTimer(std::string_view timerName) const;
    UTimer& findTimerMutable(std::string_view timerName);
};