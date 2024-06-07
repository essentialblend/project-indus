export module eu_timer;

import <chrono>;
import <string>;
import <print>;

import i_engineutility;
import ri_runnable;

export class EUTimer final : public IEngineUtility, public RIRunnable
{
public:
	explicit EUTimer() noexcept = default;
	explicit EUTimer(std::string_view name) noexcept;
	EUTimer(const EUTimer&) = default;
	EUTimer& operator=(const EUTimer&) = default;
	EUTimer(EUTimer&&) noexcept = default;
	EUTimer& operator=(EUTimer&&) noexcept = default;

	void startEntity() override;
	void stopEntity() override;
	
	void resetTimer() noexcept;

	std::chrono::duration<double> getElapsedTime() const noexcept;
	std::string getTimerName() const noexcept;
	std::string getTimerResultString() const;
	int getTimerID() const noexcept;

	~EUTimer() noexcept = default;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime{};
	std::chrono::time_point<std::chrono::high_resolution_clock> m_endTime{};
	std::string m_timerName{};
	int m_timerID{};
	bool m_isTimerRunning{ false };

	static int generateTimerID() noexcept;
};