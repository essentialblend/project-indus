export module u_timer;

import <chrono>;
import <string>;
import <print>;

export class UTimer
{
public:
	explicit UTimer() noexcept = default;
	explicit UTimer(std::string_view name) noexcept;
	UTimer(const UTimer&) = default;
	UTimer& operator=(const UTimer&) = default;
	UTimer(UTimer&&) noexcept = default;
	UTimer& operator=(UTimer&&) noexcept = default;

	void startTimer() noexcept;
	void stopTimer() noexcept;
	void resetTimer() noexcept;

	std::chrono::duration<double> getElapsedTime() const noexcept;
	std::string_view getTimerName() const noexcept;
	std::string_view getTimerResultString() const;
	int getTimerID() const noexcept;

	~UTimer() noexcept = default;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime{};
	std::chrono::time_point<std::chrono::high_resolution_clock> m_endTime{};
	std::string m_timerName{};
	int m_timerID{};
	bool m_isTimerRunning{ false };

	static int generateTimerID() noexcept;
};